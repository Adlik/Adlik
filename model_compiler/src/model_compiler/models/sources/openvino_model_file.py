# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from glob import glob
from typing import Any, Mapping, NamedTuple

from .. import repository


@repository.REPOSITORY.register_source_model
class OpenvinoModelFile(NamedTuple):
    model_path: str

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'OpenvinoModelFile':
        return OpenvinoModelFile(model_path=value['input_model'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'OpenvinoModelFile':
        return OpenvinoModelFile(model_path=env['OPENVINO_PATH'])

    @staticmethod
    def _is_openvino_file_dir(model_path: str):
        return os.path.isdir(model_path) and len(glob(os.path.join(model_path, "*.xml"))) and len(
            glob(os.path.join(model_path, "*.bin")))

    @staticmethod
    def accepts_kwargs(kwargs: Mapping[str, Any]) -> bool:
        input_model = kwargs.get('model_path')
        return isinstance(input_model, str) and OpenvinoModelFile._is_openvino_file_dir(input_model)

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')
        return isinstance(input_model, str) and OpenvinoModelFile._is_openvino_file_dir(input_model)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('OPENVINO_PATH')
        return isinstance(input_model, str) and os.path.isdir(input_model)
