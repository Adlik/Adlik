# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Sequence, Optional
import onnx
import onnx.utils
from . import repository
from .. import utilities
from ..models.sources.onnx_model_file import ONNXModelFile
from ..models.irs.onnx_model import OnnxModel
from ..models.data_format import DataFormat


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(value.get('input_formats')))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_formats = utilities.map_optional(env.get('INPUT_FORMATS'), lambda val: val.split(','))
        return Config(input_formats=utilities.get_data_formats(input_formats))


@repository.REPOSITORY.register(source_type=ONNXModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: ONNXModelFile, config: Config) -> OnnxModel:
    model = onnx.load(source.model_path)
    return OnnxModel(model, config.input_formats)
