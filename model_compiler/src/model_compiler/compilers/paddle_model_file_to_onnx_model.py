# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence
from tempfile import NamedTemporaryFile
import onnx
import onnx.utils
from paddle2onnx.command import program2onnx
from . import repository
from .. import utilities

from ..models.data_format import DataFormat
from ..models.irs.onnx_model import OnnxModel
from ..models.sources.paddle_model_file import PaddlePaddleModelFile


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]
    model_filename: Optional[str]
    params_filename: Optional[str]
    opset_version: int
    enable_onnx_checker: bool

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(value.get('input_formats')),
                      model_filename=value.get('model_filename'),
                      params_filename=value.get('params_filename'),
                      opset_version=int(value['opset_version']) if value.get('opset_version') else 9,
                      enable_onnx_checker=bool(value['enable_onnx_checker']) if value.get(
                          'enable_onnx_checker') else False
                      )

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(utilities.split_by(env.get('INPUT_FORMATS'), ',')),
                      model_filename=env.get('MODEL_FILENAME'),
                      params_filename=env.get('PARAMS_FILENAME'),
                      opset_version=int(env['OPSET_VERSION']) if env.get('OPSET_VERSION') else 9,
                      enable_onnx_checker=bool(env['ENABLE_ONNX_CHECKER']) if env.get('ENABLE_ONNX_CHECKER') else False
                      )


@repository.REPOSITORY.register(source_type=PaddlePaddleModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: PaddlePaddleModelFile, config: Config) -> OnnxModel:
    with NamedTemporaryFile(suffix='.onnx') as onnx_file:
        program2onnx(model_dir=source.model_path,
                     save_file=onnx_file.name,
                     model_filename=config.model_filename,
                     params_filename=config.params_filename,
                     opset_version=config.opset_version)
        onnx_model = onnx.load(onnx_file.name)
        graph = onnx_model.graph  # pylint: disable=no-member

    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, config.input_formats))
