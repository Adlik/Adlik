# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence

import tensorflow as tf

from . import repository
from .. import utilities
from ..models.irs.tf_model import DataFormat, Input, TensorFlowModel
from ..models.sources.tf_frozen_graph_file import FrozenGraphFile


class Config(NamedTuple):
    input_names: Optional[Sequence[str]]
    data_formats: Sequence[Optional[DataFormat]]
    output_names: Optional[Sequence[str]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value.get('input_names'),
                      data_formats=utilities.get_data_formats(value.get('input_formats')),
                      output_names=value.get('output_names'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        def _get_information(value):
            return utilities.map_optional(env.get(value), lambda val: val.split(','))

        return Config(input_names=_get_information('INPUT_NAMES'),
                      data_formats=utilities.get_data_formats(_get_information('INPUT_FORMATS')),
                      output_names=_get_information('OUTPUT_NAMES'))

    def get_input_tensors_from_graph(self, graph):
        if self.input_names is None:
            input_tensors = [operation.outputs[0]
                             for operation in graph.get_operations()
                             if operation.type == 'Placeholder']
        else:
            input_tensors = [utilities.get_tensor_by_fuzzy_name(graph, name) for name in self.input_names]

        return input_tensors

    def get_output_tensors_from_graph(self, graph):
        if self.output_names is None:
            output_tensors = [output_tensor for operation in graph.get_operations()
                              if operation.type not in
                              ['Assign', 'NoOp', 'SaveV2', 'IsVariableInitialized', 'Placeholder', 'Const']
                              for output_tensor in operation.outputs
                              if not output_tensor.consumers()]
        else:
            output_tensors = [utilities.get_tensor_by_fuzzy_name(graph, name) for name in self.output_names]

        return output_tensors


def _get_inputs(graph, config):
    input_tensors = config.get_input_tensors_from_graph(graph)
    data_formats = config.data_formats

    if len(config.data_formats) < len(input_tensors):
        data_formats.extend([None for _ in range(len(input_tensors) - len(config.data_formats))])

    return [Input(name, data_format) for name, data_format in zip(input_tensors, data_formats)]


@repository.REPOSITORY.register(source_type=FrozenGraphFile, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: FrozenGraphFile, config: Config) -> TensorFlowModel:
    graph_def = tf.compat.v1.GraphDef()

    with open(source.model_path, 'rb') as graph_file:
        graph_def.ParseFromString(graph_file.read())

    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name='')

    return TensorFlowModel(inputs=_get_inputs(graph, config),
                           outputs=config.get_output_tensors_from_graph(graph),
                           session=tf.compat.v1.Session(graph=graph, config=utilities.get_tf_cpu_only_config()))
