# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os

from . import serving_model_repository
from .compilers.repository import REPOSITORY as COMPILER_REPOSITORY
from .models.repository import REPOSITORY as MODEL_REPOSITORY
from .serving_model_repository import Config


def compile_from_json(value):
    try:
        source_type = next(model for model in MODEL_REPOSITORY.get_source_models() if model.accepts_json(value))
    except StopIteration:
        raise ValueError('Unable to determine the source model type.')

    target_type = MODEL_REPOSITORY.get_target_model(value['serving_type'])
    compiler, compiler_config_type = COMPILER_REPOSITORY.get(source_type=source_type, target_type=target_type)
    target_model = compiler(source=source_type.from_json(value), config=compiler_config_type.from_json(value))

    return serving_model_repository.save_model(Config.from_json_and_target_model(value=value,
                                                                                 target_model=target_model))


def compile_from_env():
    env = os.environ.copy()

    try:
        source_type = next(model for model in MODEL_REPOSITORY.get_source_models() if model.accepts_env(env))
    except StopIteration:
        raise ValueError('Unable to determine the source model type.')

    target_type = MODEL_REPOSITORY.get_target_model(env['serving_type'])
    compiler, compiler_config_type = COMPILER_REPOSITORY.get(source_type=source_type, target_type=target_type)
    target_model = compiler(source=source_type.from_env(env), config=compiler_config_type.from_env(env))

    return serving_model_repository.save_model(Config.from_env_and_target_model(env=env, target_model=target_model))
