# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Callable, List, Optional, TypeVar

from .models.data_format import DataFormat

_Type1 = TypeVar('_Type1')
_Type2 = TypeVar('_Type2')


def map_optional(value: Optional[_Type1], func: Callable[[_Type1], _Type2]) -> Optional[_Type2]:
    if value is None:
        return None

    return func(value)


def get_tensor_by_fuzzy_name(graph, name):
    if ':' in name:
        tensor = graph.get_tensor_by_name(name)
    else:
        tensor = graph.get_operation_by_name(name).outputs[0]

    return tensor


def get_data_formats(input_formats):
    def _get_data_format_type(model_input_format):
        return DataFormat.CHANNELS_FIRST if model_input_format == 'channels_first' else DataFormat.CHANNELS_LAST

    if input_formats:
        data_formats = [map_optional(input_format, _get_data_format_type) for input_format in input_formats]
    else:
        data_formats = []
    return data_formats


def split_by_comma(value: Optional[str]) -> Optional[List[str]]:
    return map_optional(value, lambda val: val.split(','))


def split_by(value: Optional[str], separator: str) -> Optional[List[str]]:
    return map_optional(value, lambda val: val.split(separator))
