# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional

from onnx import TypeProto, ValueInfoProto
from tensorrt import Builder, Logger, NetworkDefinitionCreationFlag, OnnxParser

from . import repository
from ..models.data_format import DataFormat
from ..models.data_type import DataType
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.tensorrt_model import Input, Output, TensorRTModel


class Config(NamedTuple):
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(value['max_batch_size'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(int(env['max_batch_size']))


def _extract_data_type_and_shape(type_proto: TypeProto):
    which_value = type_proto.WhichOneof('value')

    if which_value == 'tensor_type':
        tensor = type_proto.tensor_type
    else:
        raise ValueError

    data_type = DataType.from_onnx_data_type(tensor.elem_type)

    shape = [dim.dim_value if dim.WhichOneof('value') == 'dim_value' else None
             for dim in tensor.shape.dim[1:]]

    return data_type, shape


def _make_input(source_input: ValueInfoProto, data_format: Optional[DataFormat]) -> Input:
    data_type, shape = _extract_data_type_and_shape(source_input.type)

    return Input(name=source_input.name, data_type=data_type, shape=shape, data_format=data_format)


def _make_output(source_output: ValueInfoProto) -> Output:
    data_type, shape = _extract_data_type_and_shape(source_output.type)

    return Output(name=source_output.name, data_type=data_type, shape=shape)


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=TensorRTModel, config_type=Config)
def compile_source(source: OnnxModel, config: Config) -> TensorRTModel:
    with Logger() as logger, \
            Builder(logger) as builder, \
            builder.create_network(1 << int(NetworkDefinitionCreationFlag.EXPLICIT_BATCH)) as network, \
            OnnxParser(network, logger) as onnx_parser:
        if onnx_parser.parse(source.model_proto.SerializeToString()):
            inputs = [_make_input(source_input, data_format)
                      for source_input, data_format in zip(source.model_proto.graph.input, source.input_data_formats)]

            builder.max_batch_size = config.max_batch_size
            optimization_profile = builder.create_optimization_profile()

            for model_input in inputs:
                optimization_profile.set_shape(input=model_input.name,
                                               min=[1, *model_input.shape],
                                               opt=[(1 + config.max_batch_size) // 2, *model_input.shape],
                                               max=[config.max_batch_size, *model_input.shape])

            builder_config = builder.create_builder_config()

            builder_config.add_optimization_profile(optimization_profile)

            cuda_engine = builder.build_engine(network, builder_config)

            if cuda_engine is None:
                raise ValueError('Unable to build CUDA engine')

            return TensorRTModel(inputs=inputs,
                                 outputs=list(map(_make_output, source.model_proto.graph.output)),
                                 cuda_engine=cuda_engine)

        raise RuntimeError('\n'.join(map(str, (onnx_parser.get_error(i) for i in range(onnx_parser.num_errors)))))
