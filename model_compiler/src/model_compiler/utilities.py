# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Callable, List, Optional, TypeVar

import time
import ast
import requests
import tensorflow as tf

from .models.data_format import DataFormat

_Type1 = TypeVar('_Type1')
_Type2 = TypeVar('_Type2')


class ModelFileExtension:  # pylint: disable=too-few-public-methods
    TF_MODEL_EXTENSION = ".ckpt"


def create_named_tuple(type_class, kwargs):
    filtered_args = {}

    for field in getattr(type_class, '_fields'):
        try:
            filtered_args[field] = kwargs[field]
        except KeyError:
            pass

    return type_class(**filtered_args)


def map_optional(value: Optional[_Type1], func: Callable[[_Type1], _Type2]) -> Optional[_Type2]:
    if value is None:
        return None

    return func(value)


def get_tf_cpu_only_config():
    return tf.compat.v1.ConfigProto(device_count={'GPU': 0})


def get_data_formats(input_formats):
    def _get_data_format_type(model_input_format):
        return DataFormat.CHANNELS_FIRST if model_input_format == 'channels_first' else DataFormat.CHANNELS_LAST

    if input_formats:
        data_formats = [map_optional(input_format, _get_data_format_type) for input_format in input_formats]
    else:
        data_formats = []
    return data_formats


def get_input_shapes_from_env(env_input_shapes):
    env_input_shapes = ast.literal_eval(env_input_shapes)
    return list(env_input_shapes) if isinstance(env_input_shapes, tuple) else [env_input_shapes]


def get_onnx_model_input_data_formats(graph, input_data_formats):

    initializers = {initializer.name for initializer in graph.initializer}
    input_length = sum(1 for input_spec in graph.input if input_spec.name not in initializers)

    if input_data_formats is None:
        input_data_formats = [None for _ in range(input_length)]
    else:
        input_formats_length = len(input_data_formats)

        if input_formats_length != input_length:
            raise ValueError(
                f'Number of input formats ({input_formats_length}) does not match number of inputs ({input_length})'
            )

    return input_data_formats


def split_by(value: Optional[str], separator: str) -> Optional[List[str]]:
    return map_optional(value, lambda val: val.split(separator))


def judge_batch_size(inputs_shape, outputs_shape):
    input_batch = {input_shape[0] for input_shape in inputs_shape}
    output_batch = {output_shape[0] for output_shape in outputs_shape}
    assert input_batch == output_batch  # nosec: B101


def send_response(url, message):
    retries = 0
    max_retries = 3

    while retries <= max_retries:
        try:
            result = requests.put(url, json=message)
            return result
        except Exception:  # pylint:disable=broad-except
            time.sleep(2)
            retries += 1
            continue
