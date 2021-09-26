# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os.path import join
import shutil
from typing import Any, Mapping, NamedTuple, Optional, Sequence
from tempfile import TemporaryDirectory
from . import repository
from .. import utilities

from ..models.data_format import DataFormat
from ..models.targets.paddle_model import PaddleModel
from ..models.sources.paddle_model_file import PaddlePaddleModelFile


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]
    model_filename: Optional[str]
    params_filename: Optional[str]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(
            value.get('input_formats')),
                      model_filename=value.get('model_filename'),
                      params_filename=value.get('params_filename'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(
            utilities.split_by(env.get('INPUT_FORMATS'), ',')),
                      model_filename=env.get('MODEL_FILENAME'),
                      params_filename=env.get('PARAMS_FILENAME'))


@repository.REPOSITORY.register(source_type=PaddlePaddleModelFile,
                                target_type=PaddleModel,
                                config_type=Config)
def compile_source(source: PaddlePaddleModelFile,
                   config: Config) -> PaddleModel:
    temp_path = TemporaryDirectory()
    shutil.copy2(join(source.model_path, str(config.model_filename)),
                 join(temp_path.name, "model.pdmodel"))
    shutil.copy2(join(source.model_path, str(config.params_filename)),
                 join(temp_path.name, "model.pdiparams"))
    return PaddleModel.from_directory(temp_path, config.input_formats)
