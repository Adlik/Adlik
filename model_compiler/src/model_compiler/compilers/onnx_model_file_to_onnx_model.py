# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence

import onnx
import onnx.utils

from . import repository
from .. import utilities
from ..models import data_format
from ..models.data_format import DataFormat
from ..models.irs.onnx_model import OnnxModel
from ..models.sources.onnx_model_file import ONNXModelFile


class Config(NamedTuple):
    input_formats: Optional[Sequence[Optional[DataFormat]]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        raw_input_formats: Optional[str] = value.get('input_formats')

        return Config(input_formats=utilities.map_optional(
            raw_input_formats,
            lambda formats: list(map(data_format.str_to_data_format, formats))
        ))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.map_optional(
            utilities.split_by(env.get('INPUT_FORMATS'), ','),
            lambda formats: list(map(data_format.str_to_data_format, formats))
        ))


@repository.REPOSITORY.register(source_type=ONNXModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: ONNXModelFile, config: Config) -> OnnxModel:
    model = onnx.load(source.model_path)
    graph = model.graph  # pylint: disable=no-member
    return OnnxModel(model_proto=model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, config.input_formats))
