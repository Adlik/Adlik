# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Callable, Optional, TypeVar

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
