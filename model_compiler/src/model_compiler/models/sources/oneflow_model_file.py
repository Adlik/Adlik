# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os import path
from typing import Any, Mapping, NamedTuple
from .. import repository


@repository.REPOSITORY.register_source_model
class OneFlowModelFile(NamedTuple):
    model_path: str
    script_path: str

    @staticmethod
    def from_json(value: Mapping[str, str]) -> 'OneFlowModelFile':
        return OneFlowModelFile(model_path=value['input_model'], script_path=value.get('script_path'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'OneFlowModelFile':
        return OneFlowModelFile(model_path=env['ONEFLOW_PATH'], script_path=env.get('SCRIPT_PATH'))

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')
        input_script = kwargs.get('script_path')
        return isinstance(input_model, str) and path.isdir(input_model) and \
            isinstance(input_script, str) and path.isfile(input_script)

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')
        input_script = value.get('script_path')
        return isinstance(input_model, str) and path.isdir(input_model) and \
            isinstance(input_script, str) and path.isfile(input_script)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('ONEFLOW_PATH')
        input_script = env.get('SCRIPT_PATH')
        return isinstance(input_model, str) and path.isdir(input_model) and \
            isinstance(input_script, str) and path.isfile(input_script)
