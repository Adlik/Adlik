# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Iterable, List, Mapping, NamedTuple, Optional, Sequence

import tensorflow as tf

from . import utilities
from .models.targets.tflite_model import DataFormat


def _parse_data_type(value: str) -> tf.DType:
    try:
        data_type = getattr(tf.dtypes, value)
    except AttributeError as exception:
        raise ValueError from exception

    if isinstance(data_type, tf.DType):
        return data_type

    raise ValueError


def _parse_op_set(value: str) -> tf.lite.OpsSet:
    return tf.lite.OpsSet[value]


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]] = []
    inference_input_type: Optional[tf.DType] = tf.float32
    inference_output_type: Optional[tf.DType] = tf.float32
    optimization: bool = False
    representative_dataset: Optional[tf.lite.RepresentativeDataset] = None
    signature_keys: Optional[List[str]] = None
    supported_ops: Optional[Iterable[tf.lite.OpsSet]] = None
    supported_types: Optional[Iterable[tf.DType]] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        supported_types = value.get('supported_types')  # type: Optional[List[str]]
        supported_ops = value.get('supported_ops')  # type: Optional[List[str]]

        return Config(
            input_formats=utilities.get_data_formats(value.get('input_formats')),
            inference_input_type=utilities.map_optional(value.get('inference_input_type', 'float32'),
                                                        _parse_data_type),
            inference_output_type=utilities.map_optional(value.get('inference_output_type', 'float32'),
                                                         _parse_data_type),
            optimization=value.get('optimization', False),
            signature_keys=value.get('signature_keys'),
            supported_ops=utilities.map_optional(supported_ops, lambda items: list(map(_parse_op_set, items))),
            supported_types=utilities.map_optional(supported_types, lambda items: list(map(_parse_data_type, items))),

        )

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(
            input_formats=utilities.get_data_formats(utilities.map_optional(env.get('INPUT_FORMATS'),
                                                                            lambda val: val.split(','))),
            inference_input_type=utilities.map_optional(env.get('INFERENCE_INPUT_TYPE', 'float32'),
                                                        _parse_data_type),
            inference_output_type=utilities.map_optional(env.get('INFERENCE_OUTPUT_TYPE', 'float32'),
                                                         _parse_data_type),
            optimization=bool(int(env.get('OPTIMIZATION', '0'))),
            signature_keys=utilities.map_optional(env.get('SIGNATURE_KEYS'), lambda val: val.split(',')),
            supported_ops=utilities.map_optional(utilities.split_by(env.get('SUPPORTED_OPS'), ','),
                                                 lambda items: list(map(_parse_op_set, items))),
            supported_types=utilities.map_optional(utilities.split_by(env.get('SUPPORTED_TYPES'), ','),
                                                   lambda items: list(map(_parse_data_type, items))),

        )


def get_tflite_model(converter: tf.lite.TFLiteConverter, config: Config):
    if config.optimization:
        converter.optimizations.append(tf.lite.Optimize.DEFAULT)

    converter.representative_dataset = config.representative_dataset

    if config.supported_ops:
        converter.target_spec.supported_ops.clear()
        converter.target_spec.supported_ops.update(config.supported_ops)

    if config.supported_types:
        converter.target_spec.supported_types.extend(config.supported_types)

    converter.inference_input_type = config.inference_input_type
    converter.inference_output_type = config.inference_output_type

    tflite_model = converter.convert()
    return tflite_model
