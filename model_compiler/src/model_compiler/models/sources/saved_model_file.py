# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os import path
from typing import Any, Mapping, NamedTuple

from .. import repository


@repository.REPOSITORY.register_source_model
class SavedModelFile(NamedTuple):
    model_path: str

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'SavedModelFile':
        return SavedModelFile(model_path=value['input_model'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'SavedModelFile':
        return SavedModelFile(model_path=env['SAVED_MODEL_PATH'])

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')

        return isinstance(input_model, str) and path.isfile(input_model + '/saved_model.pb')

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')
        return isinstance(input_model, str) and path.isfile(input_model + '/saved_model.pb')

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('SAVED_MODEL_PATH')
        return isinstance(input_model, str) and path.isdir(input_model)
