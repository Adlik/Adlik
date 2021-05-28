# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import importlib.util

from typing import Any, Iterable, Mapping, NamedTuple, Optional, Sequence, Union
import tensorflow as tf
from tensorflow import keras
from .models.data_format import DataFormat
from . import utilities


def _load_module(file_path, name):
    spec = importlib.util.spec_from_file_location(name=name, location=file_path)
    module = importlib.util.module_from_spec(spec=spec)

    spec.loader.exec_module(module)

    return module


def _iterate_tensors(tensors: Union[tf.Tensor, Iterable[tf.Tensor]]):
    if isinstance(tensors, tf.Tensor):
        yield tensors
    else:
        yield tensors


def _get_data_format(tensor: tf.Tensor):
    layer = getattr(tensor, '_keras_history')[0]
    data_format = None

    for node in getattr(layer, '_outbound_nodes'):
        if tensor in _iterate_tensors(node.input_tensors):
            outbound_layer = node.outbound_layer

            try:
                keras_data_format = outbound_layer.data_format
            except AttributeError:
                continue

            if keras_data_format == 'channels_first':
                if data_format is None:
                    data_format = DataFormat.CHANNELS_FIRST
                elif data_format == DataFormat.CHANNELS_LAST:
                    return None
            else:
                if data_format is None:
                    data_format = DataFormat.CHANNELS_LAST
                elif data_format == DataFormat.CHANNELS_FIRST:
                    return None

    return data_format


def get_custom_objects(file_path):
    module_name = ''
    custom_objects = {}
    module = _load_module(file_path, module_name)

    for name, value in vars(module).items():
        if isinstance(value, type) and issubclass(value, keras.layers.Layer) and value.__module__ == module_name:
            custom_objects[name] = value

    return custom_objects


class NodeSpec(NamedTuple):
    layer_name: str
    node_index: int = 0

    @staticmethod
    def from_str(value: str) -> 'NodeSpec':
        groups = value.split(':')

        if len(groups) == 1:
            node_index = 0
        elif len(groups) == 2:
            node_index = int(groups[1])
        else:
            raise ValueError

        return NodeSpec(layer_name=groups[0], node_index=node_index)


def _get_node_specs(values: Optional[Iterable[str]]):
    return utilities.map_optional(values, lambda value: list(map(NodeSpec.from_str, value)))


def get_inputs(model: keras.Model, input_specs: Optional[Sequence[NodeSpec]]):
    if input_specs is None:
        return [(tensor, _get_data_format(tensor)) for tensor in model.inputs]

    return [(tensor, _get_data_format(tensor))
            for input_spec in input_specs
            for tensor in _iterate_tensors(model.get_layer(input_spec.layer_name).get_input_at(input_spec.node_index))]


def get_outputs(model: keras.Model, input_specs: Optional[Sequence[NodeSpec]]):
    if input_specs is None:
        return model.outputs

    return [
        tensor
        for input_spec in input_specs
        for tensor in _iterate_tensors(model.get_layer(input_spec.layer_name).get_output_at(input_spec.node_index))
    ]


class Config(NamedTuple):
    input_nodes: Optional[Sequence[NodeSpec]] = None
    output_nodes: Optional[Sequence[NodeSpec]] = None
    max_batch_size: Optional[int] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_nodes=_get_node_specs(value.get('input_layer_names')),
                      output_nodes=_get_node_specs(value.get('output_layer_names')),
                      max_batch_size=value.get('max_batch_size'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_nodes=_get_node_specs(utilities.split_by(env.get('INPUT_LAYER_NAMES'), ',')),
                      output_nodes=_get_node_specs(utilities.split_by(env.get('OUTPUT_LAYER_NAMES'), ',')),
                      max_batch_size=utilities.map_optional(env.get('max_batch_size'), int))
