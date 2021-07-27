# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import time
from collections import namedtuple

import tvm
from tvm import autotvm, te, relay
from tvm.topi import nn
from tvm.topi.x86.conv2d_avx_common import _schedule_conv_NCHWc as schedule_conv_NCHWc
from tvm.topi.x86.conv2d_avx_1x1 import _schedule_conv_NCHWc as schedule_conv_NCHWc_1x1
from tvm.autotvm.graph_tuner import DPTuner
# if tvm.__version__ == '0.7.0':
from tvm._ffi.runtime_ctypes import TVMContext as Device
# elif tvm.__version__ == '0.8.0':
#     from tvm._ffi.runtime_ctypes import Device
# else:
#    raise Exception('Not supported TVM version')

from .tvm_util import construct_conv_tensors, format_conv_workload, generate_conv_candidates


class CpuSearch:

    def __init__(self, target_name) -> None:
        self.target_name = target_name
        self.target = tvm.target.Target(target_name)
        self.kernel_records = 'kernel.log'
        self.graph_opt_sch_file = 'graph_best.log'
        self.index = 0
        self.alter_param = None

    def compile(self, model, params, schedule_file):
        if schedule_file:
            with autotvm.apply_graph_best(schedule_file):
                with tvm.transform.PassContext(opt_level=3):
                    return relay.build(model, params=params, target=self.target)
        else:
            with tvm.transform.PassContext(opt_level=3):
                return relay.build(model, params=params, target=self.target)

    def search(self, model, params, input_shape):
        tasks = autotvm.task.extract_from_program(
                model["main"], target=self.target, params=params, ops=(relay.op.get("nn.conv2d"),))

        self._tune_kernels(tasks)
        self._tune_graph(model["main"], input_shape)

        return self.graph_opt_sch_file

    def _tune_graph(self, graph, input_shape):
        target_op = [relay.op.get("nn.conv2d")]

        executor = DPTuner(graph, input_shape, self.kernel_records, target_op, self.target)
        executor.benchmark_layout_transform(min_exec_num=2000)
        executor.run()
        executor.write_opt_sch2record_file(self.graph_opt_sch_file)

    def _tune_kernels(self, tasks):

        for task in tasks:
            wkl = format_conv_workload(task.workload)
            is_1x1 = (wkl.ker_height == 1) and (wkl.ker_width == 1)
            self.alter_param = 'tile_oh' if is_1x1 else 'unroll_kw'
            candidates = generate_conv_candidates(wkl)

            self.index = 1
            for reg_n in candidates.reg_n_candidates:
                for ic_bn in candidates.ic_bn_candidates:
                    for oc_bn in candidates.oc_bn_candidates:
                        for oh_bn_or_unroll_kw in candidates.alter_candidates:
                            with self.target:
                                sch_config = (reg_n, ic_bn, oc_bn, oh_bn_or_unroll_kw)
                                conv_tensors, sch = self._create_conv_tensor_and_schedule(wkl, sch_config)
                                result = self._evaluate_schedule(wkl, conv_tensors, sch, sch_config)
                                self._write_kernel_logs(result)
                                self.index += 1

    def _create_conv_tensor_and_schedule(self, workload, sch_config):
        reg_n, ic_bn, oc_bn, oh_bn_or_unroll_kw = sch_config
        data = te.placeholder((workload.batch_num, workload.in_chn//ic_bn, workload.in_height,
                               workload.in_width, ic_bn), name="data")
        kernel = te.placeholder((workload.out_chn//oc_bn, workload.in_chn//ic_bn, workload.ker_height,
                                 workload.ker_width, ic_bn, oc_bn), name="kernel")
        conv = nn.conv2d_NCHWc(data, kernel, workload.stride, workload.padding, workload.dilation, '', '')

        sch = te.create_schedule(conv.op)

        Size = namedtuple('_Size', ['size'])
        Val = namedtuple('_Val', ['val'])
        config = {'tile_oc': Size([-1, oc_bn]), 'tile_ow': Size([-1, reg_n]),
                  self.alter_param: Val(oh_bn_or_unroll_kw)}
        is_1x1 = (workload.ker_height == 1) and (workload.ker_width == 1)
        sch = schedule_conv_NCHWc_1x1(sch, config, data, kernel, conv, conv) if is_1x1 else \
            schedule_conv_NCHWc(sch, config, data, kernel, conv, conv)

        return (data, kernel, conv), sch

    def _evaluate_schedule(self, workload, conv_tensors, sch, sch_config):

        data_array, kernel_array, c_sch = construct_conv_tensors(workload, conv_tensors, sch_config)

        time_start = time.time()
        func = tvm.build(sch, conv_tensors, target=self.target, name="conv")
        compile_time = time.time() - time_start
        evaluator = func.time_evaluator(func.entry_name, Device(1, 0), number=100, repeat=3)
        times = [evaluator(data_array, kernel_array, c_sch).mean for _ in range(8)]
        all_cost_time = compile_time + sum(times) / len(times)

        result = {"input": [self.target_name, "conv2d_NCHWc.x86",
                            [["TENSOR", [workload.batch_num, workload.in_chn,
                                         workload.in_height, workload.in_width],
                              workload.data_dtype],
                             ["TENSOR", [workload.out_chn, workload.in_chn,
                                         workload.ker_height, workload.ker_width],
                              workload.kernel_dtype],
                             workload.stride, workload.padding, workload.dilation,
                             "NCHW", "NCHW", workload.out_dtype], {}],
                  "config": {"index": self.index,
                             "code_hash": None,
                             "entity": [["tile_ic", "sp", [-1, sch_config[1]]],
                                        ["tile_oc", "sp", [-1, sch_config[2]]],
                                        ["tile_ow", "sp", [-1, sch_config[0]]],
                                        [self.alter_param, "ot", sch_config[3]]]},
                  "result": [times, 0, all_cost_time, time.time()],
                  "version": 0.2,
                  "tvm_version": tvm.__version__}

        return result

    def _write_kernel_logs(self, result):
        with open(self.kernel_records, 'a') as rec_file:
            rec_file.write(json.dumps(result))
            rec_file.write('\n')
