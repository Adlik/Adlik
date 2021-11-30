# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional

from . import repository
from .. import utilities

from .. import calibrator
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.tensorrt_model import TensorRTModel


class Config(NamedTuple):
    max_batch_size: int
    calibrator: Optional[Any] = None  # Real type should be `Optional[tensorrt.IInt8Calibrator]`.
    data_type: Optional[str] = None
    calibration_dataset: Optional[str] = None
    enable_strict_types: bool = False

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(max_batch_size=value['max_batch_size'],
                      data_type=value.get('data_type', None),
                      calibrator=value.get('calibrator', None),
                      calibration_dataset=value.get('calibration_dataset', None),
                      enable_strict_types=value.get('enable_strict_types', False))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(max_batch_size=int(env['MAX_BATCH_SIZE']),
                      data_type=env.get('DATA_TYPE', ''),
                      calibrator=env.get('CALIBRATOR', ''),
                      calibration_dataset=env.get('CALIBRATION_DATASET', ''),
                      enable_strict_types=bool(int(env.get('ENABLE_STRICT_TYPES', '0'))))


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=TensorRTModel, config_type=Config)
def compile_source(source: OnnxModel, config: Config) -> TensorRTModel:
    import tensorrt  # pylint: disable=import-outside-toplevel

    with tensorrt.Logger() as logger, \
            tensorrt.Builder(logger) as builder, \
            builder.create_network(1 << int(tensorrt.NetworkDefinitionCreationFlag.EXPLICIT_BATCH)) as network, \
            tensorrt.OnnxParser(network, logger) as onnx_parser:
        if not onnx_parser.parse(source.model_proto.SerializeToString()):
            raise ValueError('\n'.join(map(str, (onnx_parser.get_error(i) for i in range(onnx_parser.num_errors)))))

        utilities.judge_batch_size([network.get_input(i).shape for i in range(network.num_inputs)],
                                   [network.get_output(i).shape for i in range(network.num_outputs)])

        builder.max_batch_size = config.max_batch_size

        # Extract batch sizes.

        batch_sizes = {network.get_input(i).shape[0] for i in range(network.num_inputs)}

        if len(batch_sizes) > 1:
            raise ValueError('Inconsistent batch size specification.')

        # Build CUDA engine.

        builder_config = builder.create_builder_config()
        # Some layer operations require larger memory space, such as: ArgMax
        builder_config.max_workspace_size = 1 << 20

        if -1 in batch_sizes:
            optimization_profile = builder.create_optimization_profile()

            for i in range(network.num_inputs):
                input_tensor = network.get_input(i)
                shape = input_tensor.shape[1:]

                optimization_profile.set_shape(input=input_tensor.name,
                                               min=[1, *shape],
                                               opt=[(1 + config.max_batch_size) // 2, *shape],
                                               max=[config.max_batch_size, *shape])

            builder_config.add_optimization_profile(optimization_profile)

        if config.data_type == 'FP16':
            builder_config.set_flag(tensorrt.BuilderFlag.FP16)

        if config.data_type == 'INT8':
            builder_config.set_flag(tensorrt.BuilderFlag.INT8)

            if config.calibrator:
                calibrator_module = utilities.load_module(config.calibrator, 'MyCalibrator')
                builder_config.int8_calibrator = calibrator_module.MyCalibrator(config.calibration_dataset,
                                                                                config.max_batch_size)
            else:
                builder_config.int8_calibrator = calibrator.MyCalibrator(config.calibration_dataset,
                                                                         config.max_batch_size,
                                                                         [network.get_input(i).shape
                                                                          for i in range(network.num_inputs)])

        if config.enable_strict_types:
            builder_config.set_flag(tensorrt.BuilderFlag.STRICT_TYPES)

        cuda_engine = builder.build_engine(network, builder_config)

        if cuda_engine is None:
            raise ValueError('Unable to build CUDA engine')

        return TensorRTModel(cuda_engine=cuda_engine,
                             input_data_formats=list(source.input_data_formats))
