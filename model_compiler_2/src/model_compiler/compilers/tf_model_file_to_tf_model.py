# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Sequence, Tuple, Optional
import tensorflow as tf
from . import repository
from ..models.sources.tf_model_file import TfModelFile
from ..models.irs.tf_model import DataFormat, Input, TensorFlowModel


def _get_input_info(input_names, model_input_formats):
    if model_input_formats:
        input_formats = []
        for input_format in model_input_formats:
            if input_format == 'channels_first':
                input_formats.append(DataFormat.CHANNELS_FIRST)
            elif input_format == 'channels_last':
                input_formats.append(DataFormat.CHANNELS_LAST)
            else:
                input_formats.append(None)
    else:
        input_formats = [None for _ in input_names]
    if len(model_input_formats) < len(input_names):
        input_formats.extend([None for _ in range(len(input_names) - len(input_formats))])
    return list(zip(input_names, input_formats))


class Config(NamedTuple):
    input_info: Sequence[Tuple[str, Optional[DataFormat]]]
    output_names: Sequence[str]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_info=_get_input_info(value['input_names'], value['input_formats']),
                      output_names=value['output_names'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_info=_get_input_info(env['INPUT_NAMES'].split(','), env['INPUT_FORMATS'].split(',')),
                      output_names=env['OUTPUT_NAMES'].split(','))


def _get_tensor_by_fuzzy_name(graph, name):
    if ':' in name:
        tensor = graph.get_tensor_by_name(name)
    else:
        tensor = graph.get_operation_by_name(name).outputs[0]
    return tensor


def _load_model(session, model_path, config):
    tf_saver = tf.compat.v1.train.import_meta_graph(model_path + '.meta', clear_devices=True)
    tf_saver.restore(sess=session, save_path=model_path)
    input_tensors = [Input(_get_tensor_by_fuzzy_name(session.graph, name), data_format) for name, data_format in
                     config.input_info]
    output_tensors = [(_get_tensor_by_fuzzy_name(session.graph, name)) for name in config.output_names]

    return input_tensors, output_tensors


@repository.REPOSITORY.register(source_type=TfModelFile, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: TfModelFile, config: Config) -> TensorFlowModel:
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as sess:
        inputs, outputs = _load_model(sess, source.model_path, config)
    return TensorFlowModel(inputs, outputs, sess)
