# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence
from itertools import zip_longest

from . import utilities
from .models.data_format import DataFormat


def get_tensor_by_fuzzy_name(graph, name):
    if ':' in name:
        tensor = graph.get_tensor_by_name(name)
    else:
        tensor = graph.get_operation_by_name(name).outputs[0]

    return tensor


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
            input_tensors = [get_tensor_by_fuzzy_name(graph, name) for name in self.input_names]

        return input_tensors

    def get_output_tensors_from_graph(self, graph):
        if self.output_names is None:
            output_tensors = [output_tensor for operation in graph.get_operations()
                              if operation.type not in
                              ['Assign', 'Const', 'Identity', 'IsVariableInitialized', 'NoOp', 'Placeholder', 'SaveV2',
                               'VarIsInitializedOp']
                              for output_tensor in operation.outputs
                              if not output_tensor.consumers()]
        else:
            output_tensors = [get_tensor_by_fuzzy_name(graph, name) for name in self.output_names]

        return output_tensors


def get_inputs(graph, config):
    return zip_longest(config.get_input_tensors_from_graph(graph), config.data_formats)
