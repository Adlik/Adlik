# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os import path
from typing import Any, Mapping, NamedTuple, Optional
from .. import repository


@repository.REPOSITORY.register_source_model
class TorchModelFile(NamedTuple):
    model_path: str
    script_path: Optional[str] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'TorchModelFile':
        return TorchModelFile(model_path=value['input_model'], script_path=value.get('script_path'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'TorchModelFile':
        return TorchModelFile(model_path=env['TORCH_PATH'], script_path=env.get('SCRIPT_PATH'))

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')
        return isinstance(input_model, str) and input_model[-4:].upper() == '.PTH' and path.isfile(input_model)

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')

        return isinstance(input_model, str) and input_model.lower().endswith('.pth') and path.isfile(input_model)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('TORCH_PATH')
        return isinstance(input_model, str) and path.isfile(input_model)
