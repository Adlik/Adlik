# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, List, Mapping, NamedTuple, Optional, Sequence
import torch
from .models.data_type import DataType
from .models.data_format import DataFormat
from . import utilities


class Config(NamedTuple):
    input_names: Sequence[str]
    input_shapes: List[List]
    data_type: torch.dtype     # pylint: disable=no-member
    max_batch_size: int
    input_formats: Sequence[Optional[DataFormat]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value['input_names'],
                      input_shapes=utilities.get_input_shapes(value.get('input_shapes')),
                      data_type=value['data_type'],
                      max_batch_size=value['max_batch_size'],
                      input_formats=utilities.get_data_formats(value.get('input_formats')))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_shapes = utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES'))
        data_type = DataType.from_torch_data_type(env.get('DATA_TYPE'))

        return Config(input_names=env['INPUT_NAMES'].split(','),
                      input_shapes=input_shapes,
                      data_type=data_type,
                      max_batch_size=int(env['MAX_BATCH_SIZE']),
                      input_formats=utilities.get_data_formats(utilities.split_by(env.get('INPUT_FORMATS'), ',')))
