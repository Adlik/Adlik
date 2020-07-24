# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence, Tuple
import tensorflow as tf
from . import repository
from ..utilities import split_by_comma, get_tensor_by_fuzzy_name
from ..models.sources.tf_frozen_graph_file import FrozenGraphFile
from ..models.targets.openvino_model import OpenvinoModel
from ..models.data_format import DataFormat, str_to_data_format, as_model_config_data_format
from ..protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _str_to_data_formats(str_formats: Sequence[str]):
    return [str_to_data_format(str_format) for str_format in str_formats]


def _get_input_info(input_names, str_formats):
    if input_names is None:
        return None
    if str_formats is None:
        return list(zip(input_names, [None for _ in range(len(input_names))]))
    if len(input_names) == len(str_formats):
        return list(zip(input_names, _str_to_data_formats(str_formats)))
    raise ValueError('Input names and formats should be have the same length or have no formats')


class Config(NamedTuple):
    input_info: Optional[Sequence[Tuple[Optional[str], Optional[DataFormat]]]]
    output_names: Optional[Sequence[str]]
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        input_names = value.get('input_names', None)
        input_formats = value.get('input_formats', None)
        output_names = value.get('output_names', None)
        max_batch_size = value.get('max_batch_size', 1)
        input_info = _get_input_info(input_names, input_formats)
        return Config(input_info=input_info,
                      output_names=output_names,
                      max_batch_size=max_batch_size)

    @staticmethod
    def from_env(env: Mapping[str, Any]) -> 'Config':
        input_names = split_by_comma(env.get('INPUT_NAMES', None))
        input_formats = split_by_comma(env.get('INPUT_FORMATS', None))
        output_names = split_by_comma(env.get('OUTPUT_NAMES', None))
        max_batch_size = env.get('MAX_BATCH_SIZE', 1)
        input_info = _get_input_info(input_names, input_formats)
        print(input_info)
        return Config(input_info=input_info,
                      output_names=output_names,
                      max_batch_size=max_batch_size)


def _get_inputs(graph, config):
    if config.input_info is None:
        return None
    inputs = []
    for item in config.input_info:
        tensor = get_tensor_by_fuzzy_name(graph, item[0])
        # OpenVINO only support NCHW, so should transpose shape if data_format is 'channels_last'
        dims = [-1 if dim is None else dim for dim in tensor.shape[1:]]
        data_format = item[1]
        if data_format == DataFormat.CHANNELS_LAST:
            data_format = DataFormat.CHANNELS_FIRST
            channel = dims.pop(-1)
            dims.insert(0, channel)
        inputs.append(ModelInput(name=item[0],
                                 data_type=tensor.dtype.as_datatype_enum,
                                 format=as_model_config_data_format(data_format),
                                 dims=dims))
    return inputs


def _get_outputs(graph, config):
    outputs = []
    for name in config.output_names:
        tensor = get_tensor_by_fuzzy_name(graph, name)
        outputs.append(ModelOutput(name=name,
                                   data_type=tensor.dtype.as_datatype_enum,
                                   dims=[-1 if dim is None else dim for dim in tensor.shape[1:]]))
    return outputs


@repository.REPOSITORY.register(source_type=FrozenGraphFile, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: FrozenGraphFile, config: Config) -> OpenvinoModel:
    graph_def = tf.compat.v1.GraphDef()

    with open(source.model_path, 'rb') as graph_file:
        graph_def.ParseFromString(graph_file.read())

    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name='')

        inputs = _get_inputs(graph, config)
        outputs = _get_outputs(graph, config)
    return OpenvinoModel(inputs, outputs, source.model_path, config.max_batch_size)
