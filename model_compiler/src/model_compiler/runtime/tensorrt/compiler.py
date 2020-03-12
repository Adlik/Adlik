# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
TensorRT model compiler
"""

import os

from model_compiler import log_util
import numpy as np
import tensorrt as trt  # pylint:disable=import-error
import uff  # pylint:disable=import-error

from ..compiler_base import BaseCompiler

_LOGGER = log_util.get_logger(__name__)

TRT_LOGGER = trt.Logger(trt.Logger.INFO)


class Compiler(BaseCompiler):
    """
    TensorRT model compiler
    """

    def __init__(self, config):
        super(Compiler, self).__init__(config)
        self.uff_path = os.path.join(self.model_dir, 'model.uff')
        os.makedirs(self.target_dir, exist_ok=True)
        self.plan_path = os.path.join(self.target_dir, 'model.plan')
        self.max_workspace_size_byte = 1 << 25
        self.frozen_pb_path = os.path.join(self.model_dir, 'frozen.pb')

    def _after_load_model(self, session, inputs, outputs):
        return self._to_frozen_graph(session, self.frozen_pb_path, outputs)

    def _after_end_session(self, model_info):
        if model_info.source_type == 'ONNX':
            self._parser_model_onnx(model_info)
        else:
            self._pb_to_uff(self.frozen_pb_path, model_info)
            self._parser_model_uff(model_info)
            os.remove(self.uff_path)

    def _pb_to_uff(self, frozen_graph_path, model_info):
        input_node = list()
        for i in model_info.inputs:
            input_node.append(",".join([i.name, i.name, str(np.dtype(i.dtype.as_numpy_dtype)),
                                        ",".join(str(i) for i in i.shape)]))
        uff_model = uff.from_tensorflow_frozen_model(frozen_graph_path, [o.name for o in model_info.outputs],
                                                     output_filename=self.uff_path, input_node=input_node)
        _LOGGER.info('pb_to_uff:: convert to uff success, output file: %s', self.uff_path)
        return uff_model

    def _parser_model(self, builder, network, model_info):
        builder.max_workspace_size = self.max_workspace_size_byte
        builder.max_batch_size = model_info.max_batch_size

        _LOGGER.info('model_to_plan:: Begin to build engine!')
        engine = builder.build_cuda_engine(network)
        if engine is None:
            raise Exception('build cuda engine from network failed, engine is None')

        _LOGGER.info('model_to_plan:: Build engine: %s, begin to serialize engine!', engine)

        with open(self.plan_path, "wb") as plan_file:
            plan_file.write(engine.serialize())
        _LOGGER.info('model_to_plan:: serialize engine success!')

        # update outputs' shape
        for output in model_info.outputs:
            output.shape = engine.get_binding_shape(output.name)
            _LOGGER.info('Update output from engine: "%s" shape: %s', output.name, output.shape)

    def _parser_model_uff(self, model_info):
        with trt.Builder(TRT_LOGGER) as builder, builder.create_network() as network, trt.UffParser() as parser:

            for i in model_info.inputs:
                # tensorRT only support NCHW, so should transpose shape if data_format is 'channels_last'
                if i.data_format == 'channels_last':
                    channel = i.shape.pop(-1)
                    i.shape.insert(0, channel)
                    i.data_format = 'channels_first'
                    _LOGGER.info('model_to_plan:: convert input %s to channel first format, shape: %s', i.name, i.shape)
                    result = parser.register_input(i.name, i.shape)
                elif i.data_format == 'channels_first':
                    result = parser.register_input(i.name, i.shape)
                else:
                    result = parser.register_input(i.name, i.shape)
                _LOGGER.info('model_to_plan:: register input to trt parser: name: %s, shape: %s, result: %s',
                             i.name, i.shape, result)

            for output in model_info.outputs:
                _LOGGER.info('model_to_plan:: register output to trt parser: name: %s', output.name)
                parser.register_output(output.name)
            _LOGGER.info('model_to_plan:: Begin to parse network!')
            result = parser.parse(self.uff_path, network)
            if not result:
                raise Exception('model_to_plan:: Parse network from uff file failure!')

            self._parser_model(builder=builder, network=network, model_info=model_info)

    def _parser_model_onnx(self, model_info):
        g_logger = trt.Logger(trt.Logger.WARNING)
        with trt.Builder(g_logger) as builder, builder.create_network() as network, \
                trt.OnnxParser(network, g_logger) as parser:

            for i in model_info.inputs:
                if i.data_format == 'channels_last':
                    raise Exception('The data format: {} is not support'.format(i.data_format))

            _LOGGER.info('model_to_plan:: Begin to parse network!')
            with open(self.model_path, 'rb') as model:
                result = parser.parse(model.read())
            if not result:
                raise Exception('model_to_plan:: Parse network from uff file failure!')

            self._parser_model(builder=builder, network=network, model_info=model_info)

    def get_platform(self):
        """
        Get platform
        :return:
        """
        return "tensorrt_plan", trt.__version__
