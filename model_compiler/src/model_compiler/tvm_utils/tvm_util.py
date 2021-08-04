# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from collections import namedtuple

import numpy as np
from tvm import topi
import tvm


def format_conv_workload(workload_ori):

    Workload = namedtuple('Workload', ['batch_num', 'in_chn', 'in_height', 'in_width', 'out_chn',
                                       'ker_height', 'ker_width', 'stride', 'padding', 'dilation',
                                       'data_dtype', 'kernel_dtype', 'out_dtype'])
    workload = Workload(batch_num=workload_ori[1][1][0], in_chn=workload_ori[1][1][1], in_height=workload_ori[1][1][2],
                        in_width=workload_ori[1][1][3], out_chn=workload_ori[2][1][0], ker_height=workload_ori[2][1][2],
                        ker_width=workload_ori[2][1][3], stride=workload_ori[3], padding=workload_ori[4],
                        dilation=workload_ori[5], data_dtype=workload_ori[1][2], kernel_dtype=workload_ori[2][2],
                        out_dtype=workload_ori[-1])
    return workload


def generate_exp_candidates(max_num):
    candidates = [1]
    i = 2
    while i < max_num:
        ele = 2*i if 2*i <= max_num else max_num
        candidates.append(ele)
        i *= 2
    return candidates


def generate_conv_candidates(workload):

    reg_n_candidates = [2, 4, 8, 16, 32]
    unroll_candidates = [True, False]
    oh_bn_candidates = [1, 2]
    ic_bn_candidates = generate_exp_candidates(workload.in_chn)
    oc_bn_candidates = generate_exp_candidates(workload.out_chn)

    is_1x1 = (workload.ker_height == 1) and (workload.ker_width == 1)
    alter_candidates = oh_bn_candidates if is_1x1 else unroll_candidates
    Candidates = namedtuple('Candidates', ['reg_n_candidates', 'ic_bn_candidates',
                                           'oc_bn_candidates', 'alter_candidates'])
    return Candidates(reg_n_candidates=reg_n_candidates, ic_bn_candidates=ic_bn_candidates,
                      oc_bn_candidates=oc_bn_candidates, alter_candidates=alter_candidates)


def construct_conv_tensors(workload, conv_tensors, sch_config):
    data_shape = (workload.batch_num, workload.in_chn//sch_config[1], workload.in_height,
                  workload.in_width, sch_config[1])
    kernel_shape = (workload.out_chn//sch_config[2], workload.in_chn//sch_config[1],
                    workload.ker_height, workload.ker_width,
                    sch_config[1], sch_config[2])
    o_shape = [int(i) for i in topi.nn.relu(conv_tensors[-1]).shape]
    data_array = tvm.nd.array(np.random.rand(*data_shape).astype(dtype=workload.data_dtype))
    kernel_array = tvm.nd.array(np.random.rand(*kernel_shape).astype(dtype=workload.kernel_dtype))
    c_sch = tvm.nd.array(np.zeros(o_shape, dtype=workload.out_dtype))

    return data_array, kernel_array, c_sch
