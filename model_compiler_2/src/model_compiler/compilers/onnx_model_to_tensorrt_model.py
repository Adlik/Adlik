# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple

from onnx import TensorShapeProto, TypeProto
from tensorrt import Builder, Logger, NetworkDefinitionCreationFlag, OnnxParser

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.tensorrt_model import TensorRTModel


class Config(NamedTuple):
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(value['max_batch_size'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(int(env['MAX_BATCH_SIZE']))


def _extract_dimension(dim: TensorShapeProto.Dimension):
    return dim.dim_value if dim.WhichOneof('value') == 'dim_value' else None


def _extract_shape(type_proto: TypeProto):
    if type_proto.WhichOneof('value') == 'tensor_type':
        return list(map(_extract_dimension, type_proto.tensor_type.shape.dim))

    raise ValueError


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=TensorRTModel, config_type=Config)
def compile_source(source: OnnxModel, config: Config) -> TensorRTModel:
    with Logger() as logger, \
            Builder(logger) as builder, \
            builder.create_network(1 << int(NetworkDefinitionCreationFlag.EXPLICIT_BATCH)) as network, \
            OnnxParser(network, logger) as onnx_parser:
        if not onnx_parser.parse(source.model_proto.SerializeToString()):
            raise ValueError('\n'.join(map(str, (onnx_parser.get_error(i) for i in range(onnx_parser.num_errors)))))

        builder.max_batch_size = config.max_batch_size

        # Extract batch sizes.

        batch_sizes = set()
        name_and_shapes = []

        for model_input in source.get_inputs():
            batch_size, *shape = _extract_shape(model_input.type)

            batch_sizes.add(batch_size)
            name_and_shapes.append((model_input.name, shape))

        if len(batch_sizes) > 1:
            raise ValueError('Inconsistent batch size specification.')

        # Build CUDA engine.

        builder_config = builder.create_builder_config()

        if None in batch_sizes:
            optimization_profile = builder.create_optimization_profile()

            for (name, shape) in name_and_shapes:
                optimization_profile.set_shape(input=name,
                                               min=[1, *shape],
                                               opt=[(1 + config.max_batch_size) // 2, *shape],
                                               max=[config.max_batch_size, *shape])

            builder_config.add_optimization_profile(optimization_profile)

        cuda_engine = builder.build_engine(network, builder_config)

        if cuda_engine is None:
            raise ValueError('Unable to build CUDA engine')

        return TensorRTModel(cuda_engine=cuda_engine,
                             input_data_formats=list(source.input_data_formats))
