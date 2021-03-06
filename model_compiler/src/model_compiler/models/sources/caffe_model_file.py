# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Any, Mapping, NamedTuple

from .. import repository


@repository.REPOSITORY.register_source_model
class CaffeModelFile(NamedTuple):
    model_path: str

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'CaffeModelFile':
        return CaffeModelFile(model_path=value['input_model'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'CaffeModelFile':
        return CaffeModelFile(model_path=env['CAFFE_PATH'])

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')

        return isinstance(input_model, str) and os.path.isfile(input_model + '/predict_net.pb')

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')

        return isinstance(input_model, str) and os.path.isfile(input_model + '/predict_net.pb')

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('CAFFE_PATH')

        return isinstance(input_model, str) and os.path.isdir(input_model)
