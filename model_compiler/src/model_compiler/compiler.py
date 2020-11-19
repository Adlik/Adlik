# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Optional

from . import serving_model_repository, utilities
from .compilers.repository import REPOSITORY as COMPILER_REPOSITORY
from .models.repository import REPOSITORY as MODEL_REPOSITORY
from .serving_model_repository import Config


def compile_model(serving_type: str,
                  model_name: str,
                  max_batch_size: int,
                  export_path: str,
                  version: Optional[int] = None,
                  **kwargs):
    value = kwargs
    value.update({'model_name': model_name, 'max_batch_size': max_batch_size})

    try:
        source_type = next(model for model in MODEL_REPOSITORY.get_source_models() if model.accepts_kwargs(value))
    except StopIteration as exception:
        raise ValueError('Unable to determine the source model type.') from exception

    target_type = MODEL_REPOSITORY.get_target_model(serving_type)
    compiler, compiler_config_type = COMPILER_REPOSITORY.get(source_type=source_type, target_type=target_type)

    target_model = compiler(source=utilities.create_named_tuple(source_type, value),
                            config=compiler_config_type.from_kwargs(**value))

    return serving_model_repository.save_model(Config.from_target_model(target_model=target_model,
                                                                        model_name=model_name,
                                                                        max_batch_size=max_batch_size,
                                                                        export_path=export_path,
                                                                        version=version))


def compile_from_json(value):
    value.update({'model_name': value['model_name'], 'max_batch_size': value['max_batch_size']})

    try:
        source_type = next(model for model in MODEL_REPOSITORY.get_source_models() if model.accepts_json(value))
    except StopIteration as exception:
        raise ValueError('Unable to determine the source model type.') from exception

    target_type = MODEL_REPOSITORY.get_target_model(value['serving_type'])
    compiler, compiler_config_type = COMPILER_REPOSITORY.get(source_type=source_type, target_type=target_type)
    target_model = compiler(source=source_type.from_json(value), config=compiler_config_type.from_json(value))

    return serving_model_repository.save_model(Config.from_target_model(target_model=target_model,
                                                                        model_name=value['model_name'],
                                                                        max_batch_size=value['max_batch_size'],
                                                                        export_path=value['export_path'],
                                                                        version=value.get('version')))


def compile_from_env():
    env = os.environ.copy()

    try:
        source_type = next(model for model in MODEL_REPOSITORY.get_source_models() if model.accepts_env(env))
    except StopIteration as exception:
        raise ValueError('Unable to determine the source model type.') from exception

    target_type = MODEL_REPOSITORY.get_target_model(env['SERVING_TYPE'])
    compiler, compiler_config_type = COMPILER_REPOSITORY.get(source_type=source_type, target_type=target_type)
    target_model = compiler(source=source_type.from_env(env), config=compiler_config_type.from_env(env))

    return serving_model_repository.save_model(Config.from_env_and_target_model(env=env, target_model=target_model))
