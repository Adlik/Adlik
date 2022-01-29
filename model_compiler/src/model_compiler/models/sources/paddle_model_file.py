# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from glob import glob
from os import path
from typing import Any, Mapping, NamedTuple

from .. import repository


@repository.REPOSITORY.register_source_model
class PaddlePaddleModelFile(NamedTuple):
    model_path: str

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'PaddlePaddleModelFile':
        return PaddlePaddleModelFile(model_path=value['input_model'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'PaddlePaddleModelFile':
        return PaddlePaddleModelFile(model_path=env['PADDLE_PADDLE_PATH'])

    @staticmethod
    def _is_paddle_file_dir(model_path: str):
        return path.isdir(model_path) and len(glob(path.join(model_path, "*.pdiparams"))) and (
               len(glob(path.join(model_path, "*.pdmodel"))) or
               path.exists(path.join(model_path, "__model__")))

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')

        return isinstance(input_model, str) and path.isdir(input_model) and \
            PaddlePaddleModelFile._is_paddle_file_dir(input_model)

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')

        return isinstance(input_model, str) and path.isdir(input_model) and \
            PaddlePaddleModelFile._is_paddle_file_dir(input_model)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('PADDLE_PADDLE_PATH')

        return isinstance(input_model, str) and path.isdir(input_model)
