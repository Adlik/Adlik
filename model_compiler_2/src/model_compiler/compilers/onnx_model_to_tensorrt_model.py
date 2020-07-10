# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple

from onnx import TypeProto
from tensorrt import Builder, Logger, NetworkDefinitionCreationFlag, OnnxParser

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.tensorrt_model import Input, TensorRTModel


class Config(NamedTuple):
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(value['max_batch_size'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(int(env['MAX_BATCH_SIZE']))


def _extract_shape(type_proto: TypeProto):
    which_value = type_proto.WhichOneof('value')

    if which_value == 'tensor_type':
        tensor = type_proto.tensor_type
    else:
        raise ValueError

    return [dim.dim_value if dim.WhichOneof('value') == 'dim_value' else None for dim in tensor.shape.dim[1:]]


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=TensorRTModel, config_type=Config)
def compile_source(source: OnnxModel, config: Config) -> TensorRTModel:
    with Logger() as logger, \
            Builder(logger) as builder, \
            builder.create_network(1 << int(NetworkDefinitionCreationFlag.EXPLICIT_BATCH)) as network, \
            OnnxParser(network, logger) as onnx_parser:
        if onnx_parser.parse(source.model_proto.SerializeToString()):
            inputs = [Input(name=source_input.name, data_format=data_format)
                      for source_input, data_format in zip(source.model_proto.graph.input, source.input_data_formats)]

            builder.max_batch_size = config.max_batch_size
            optimization_profile = builder.create_optimization_profile()
            builder_config = builder.create_builder_config()

            for model_input in source.model_proto.graph.input:
                shape = _extract_shape(model_input.type)

                optimization_profile.set_shape(input=model_input.name,
                                               min=[1, *shape],
                                               opt=[(1 + config.max_batch_size) // 2, *shape],
                                               max=[config.max_batch_size, *shape])

            builder_config.add_optimization_profile(optimization_profile)

            cuda_engine = builder.build_engine(network, builder_config)

            if cuda_engine is None:
                raise ValueError('Unable to build CUDA engine')

            return TensorRTModel(inputs=inputs,
                                 outputs=[output.name for output in source.model_proto.graph.output],
                                 cuda_engine=cuda_engine)

        raise RuntimeError('\n'.join(map(str, (onnx_parser.get_error(i) for i in range(onnx_parser.num_errors)))))
