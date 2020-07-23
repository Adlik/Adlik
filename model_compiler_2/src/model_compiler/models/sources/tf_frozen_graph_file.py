# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os import path
from typing import Any, Mapping, NamedTuple

from .. import repository


@repository.REPOSITORY.register_source_model
class FrozenGraphFile(NamedTuple):
    model_path: str

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'FrozenGraphFile':
        return FrozenGraphFile(model_path=value['input_model'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'FrozenGraphFile':
        return FrozenGraphFile(model_path=env['FROZEN_GRAPH_PATH'])

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')
        return isinstance(input_model, str) and input_model.lower().endswith('.pb') and path.isfile(input_model)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('FROZEN_GRAPH_PATH')
        return isinstance(input_model, str) and path.isfile(input_model)
