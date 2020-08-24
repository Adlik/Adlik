# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from typing import Any, Iterable, List, Mapping, NamedTuple, Optional

import tensorflow as tf

from . import repository
from .. import utilities
from ..models.targets.saved_model import SavedModel
from ..models.targets.tflite_model import TfLiteModel


def _parse_data_type(value: str):
    try:
        data_type = getattr(tf.dtypes, value)
    except AttributeError as exception:
        raise ValueError from exception

    if isinstance(data_type, tf.DType):
        return data_type

    raise ValueError


class Config(NamedTuple):
    optimization: bool = False
    supported_types: Optional[Iterable[tf.DType]] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        supported_types = value.get('supported_types')  # type: Optional[List[str]]

        return Config(optimization=value.get('optimization', False),
                      supported_types=utilities.map_optional(supported_types,
                                                             lambda items: list(map(_parse_data_type, items))))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(optimization=bool(int(env.get('OPTIMIZATION', '0'))),
                      supported_types=utilities.map_optional(utilities.split_by(env.get('SUPPORTED_TYPES'), ','),
                                                             lambda items: list(map(_parse_data_type, items))))


@repository.REPOSITORY.register(source_type=SavedModel, target_type=TfLiteModel, config_type=Config)
def compile_source(source: SavedModel, config: Config) -> TfLiteModel:
    with TemporaryDirectory() as directory:
        source.save(directory)

        converter = tf.lite.TFLiteConverter.from_saved_model(directory)

    if config.optimization:
        converter.optimizations.append(tf.lite.Optimize.DEFAULT)

    if config.supported_types:
        converter.target_spec.supported_types.extend(config.supported_types)

    tflite_model = converter.convert()
    input_formats = [model_input.data_format for model_input in source.inputs]

    return TfLiteModel(tflite_model, input_formats)
