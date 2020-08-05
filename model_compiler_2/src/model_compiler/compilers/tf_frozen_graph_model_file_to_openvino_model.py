# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from typing import Any, Mapping, NamedTuple, Optional, Sequence, Tuple

import tensorflow as tf

from . import repository
from ..models.data_format import DataFormat, as_model_config_data_format, str_to_data_format
from ..models.sources.tf_frozen_graph_file import FrozenGraphFile
from ..models.targets.openvino_model import OpenvinoModel
from ..openvino_util import OptParams, convert
from ..protos.generated.model_config_pb2 import ModelInput, ModelOutput
from ..utilities import get_tensor_by_fuzzy_name, split_by_comma


def _get_input_info(input_names, str_formats):
    if input_names is None:
        return None
    if str_formats is None:
        return [(input_name, None) for input_name in input_names]
    if len(input_names) == len(str_formats):
        return list(zip(input_names, map(str_to_data_format, str_formats)))
    raise ValueError('Input names and formats should be have the same length or have no formats')


class Config(NamedTuple):
    input_info: Optional[Sequence[Tuple[str, Optional[DataFormat]]]]
    output_names: Optional[Sequence[str]]
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        input_names = value.get('input_names')
        input_formats = value.get('input_formats')
        output_names = value.get('output_names')
        max_batch_size = value.get('max_batch_size', 1)
        input_info = _get_input_info(input_names, input_formats)
        return Config(input_info=input_info,
                      output_names=output_names,
                      max_batch_size=max_batch_size)

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_names = split_by_comma(env.get('INPUT_NAMES'))
        input_formats = split_by_comma(env.get('INPUT_FORMATS'))
        output_names = split_by_comma(env.get('OUTPUT_NAMES'))
        max_batch_size = env.get('MAX_BATCH_SIZE', 1)
        input_info = _get_input_info(input_names, input_formats)
        return Config(input_info=input_info,
                      output_names=output_names,
                      max_batch_size=int(max_batch_size))


def _get_inputs(graph, config):
    if config.input_info is None:
        return None
    inputs = []
    for name, data_format in config.input_info:
        tensor = get_tensor_by_fuzzy_name(graph, name)
        # OpenVINO only support NCHW, so should transpose shape if data_format is 'channels_last'
        dims = [-1 if dim is None else dim for dim in tensor.shape[1:]]
        if data_format == DataFormat.CHANNELS_LAST:
            data_format = DataFormat.CHANNELS_FIRST
            channel = dims.pop(-1)
            dims.insert(0, channel)
        inputs.append(ModelInput(name=name,
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

    temp_path = TemporaryDirectory()
    convert(OptParams(source_path=source.model_path,
                      target_path=temp_path.name,
                      max_batch_size=config.max_batch_size,
                      inputs=inputs,
                      outputs=outputs),
            'mo_tf.py')
    return OpenvinoModel(inputs, outputs, temp_path)
