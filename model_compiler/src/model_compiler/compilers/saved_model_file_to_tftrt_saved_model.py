# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence
from itertools import zip_longest

from tensorflow.python.compiler.tensorrt import trt_convert as trt  # pylint: disable=no-name-in-module
from tensorflow.python.saved_model import signature_constants, tag_constants  # pylint: disable=no-name-in-module
from tensorflow.python.tools import saved_model_cli, saved_model_utils  # pylint: disable=no-name-in-module

from . import repository
from .. import utilities
from ..models.data_format import DataFormat
from ..models.sources.saved_model_file import SavedModelFile
from ..models.targets.tftrt_saved_model import TfTRTSavedModel, Input, Output


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]
    max_batch_size: int
    signature_keys: Optional[str] = None
    enable_fp16: bool = False
    enable_int8: bool = False
    optimize_offline: bool = False
    calibration_input_fn: Optional[Any] = None
    input_fn: Optional[Any] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(signature_keys=value.get('signature_keys'),
                      input_formats=utilities.get_data_formats(value.get('input_formats')),
                      max_batch_size=value['max_batch_size'],
                      optimize_offline=value.get('optimize_offline', False),
                      enable_fp16=value.get('enable_fp16', False))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        def _get_information(value):
            return utilities.map_optional(env.get(value), lambda val: val.split(','))

        return Config(signature_keys=env.get('SIGNATURE_KEYS'),
                      input_formats=utilities.get_data_formats(_get_information('INPUT_FORMATS')),
                      max_batch_size=int(env['MAX_BATCH_SIZE']),
                      optimize_offline=bool(int(env.get('OPTIMIZE_OFFLINE', '0'))),
                      enable_fp16=bool(int(env.get('ENABLE_FP16', '0'))))


def _get_saved_model_file_inputs(meta_graph_def, signature_def_key):
    inputs_tensor_info = saved_model_cli._get_inputs_tensor_info_from_meta_graph_def(  # pylint: disable=W0212
        meta_graph_def=meta_graph_def, signature_def_key=signature_def_key)
    return len(inputs_tensor_info)


def _get_inputs(meta_graph_def, signature_def_key, input_data_formats):
    inputs_tensor_info = saved_model_cli._get_inputs_tensor_info_from_meta_graph_def(  # pylint: disable=W0212
        meta_graph_def=meta_graph_def, signature_def_key=signature_def_key)

    input_names = []
    input_tensors = []
    for input_name, input_tensor in sorted(inputs_tensor_info.items()):
        input_names.append(input_name)
        input_tensors.append(input_tensor)
    input_info = zip_longest(input_names, input_tensors, input_data_formats)

    return [Input(input_name, input_tensor, data_format)
            for input_name, input_tensor, data_format in input_info]


def _get_outputs(meta_graph_def, signature_def_key):
    outputs_tensor_info = saved_model_cli._get_outputs_tensor_info_from_meta_graph_def(  # pylint: disable=W0212
        meta_graph_def=meta_graph_def, signature_def_key=signature_def_key)

    return [Output(output_name, output_tensor)
            for output_name, output_tensor in sorted(outputs_tensor_info.items())]


def _get_trt_conversion_params(precision_mode, max_batch_size):
    conversion_params = trt.DEFAULT_TRT_CONVERSION_PARAMS
    conversion_params = conversion_params._replace(precision_mode=precision_mode)
    conversion_params = conversion_params._replace(use_calibration=precision_mode == 'INT8')
    conversion_params = conversion_params._replace(max_batch_size=max_batch_size)
    return conversion_params


@repository.REPOSITORY.register(source_type=SavedModelFile, target_type=TfTRTSavedModel, config_type=Config)
def compile_source(source: SavedModelFile, config: Config) -> TfTRTSavedModel:
    meta_graph_def = saved_model_utils.get_meta_graph_def(source.model_path, tag_constants.SERVING)
    signature_def_key = signature_constants.DEFAULT_SERVING_SIGNATURE_DEF_KEY
    if config.signature_keys:
        signature_def_key = config.signature_keys

    input_length = _get_saved_model_file_inputs(meta_graph_def, signature_def_key)
    input_data_formats = config.input_formats
    if input_data_formats is None:
        input_data_formats = [None for _ in range(input_length)]
    else:
        input_formats_length = len(input_data_formats)

        if input_formats_length != input_length:
            raise ValueError(
                f'Number of input formats ({input_formats_length}) does not match number of inputs ({input_length})'
            )

    precision = 'FP32'
    max_batch_size = config.max_batch_size

    if config.enable_fp16:
        precision = 'FP16'

    if config.enable_int8:
        precision = 'INT8'

    params = _get_trt_conversion_params(precision_mode=precision, max_batch_size=max_batch_size)

    input_saved_model_signature_key = signature_constants.DEFAULT_SERVING_SIGNATURE_DEF_KEY
    if config.signature_keys:
        input_saved_model_signature_key = config.signature_keys

    converter = trt.TrtGraphConverterV2(input_saved_model_dir=source.model_path,
                                        input_saved_model_tags=tag_constants.SERVING,
                                        input_saved_model_signature_key=input_saved_model_signature_key,
                                        conversion_params=params)

    if config.enable_int8:
        converter.convert(calibration_input_fn=config.calibration_input_fn)
        if config.optimize_offline:
            converter.build(input_fn=config.input_fn)
        tftrt_saved_model = converter

    else:
        converter.convert()
        if config.optimize_offline:
            converter.build(input_fn=config.input_fn)  # pylint: disable=no-member
        tftrt_saved_model = converter
    inputs = _get_inputs(meta_graph_def, signature_def_key, input_data_formats)
    outputs = _get_outputs(meta_graph_def, signature_def_key)

    return TfTRTSavedModel(tftrt_saved_model, inputs=inputs, outputs=outputs)
