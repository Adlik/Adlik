# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import zipfile
from os import path
from typing import Any, Mapping, NamedTuple, Optional
from zipfile import ZipFile

import google.protobuf.text_format

from . import utilities
from .protos.generated.model_config_pb2 import ModelConfigProto


class Config(NamedTuple):
    model: Any
    model_config: ModelConfigProto
    path: str
    version: Optional[int] = None

    @staticmethod
    def from_json_and_target_model(value: Mapping[str, Any], target_model):
        platform, platform_version = target_model.get_platform()

        return Config(model=target_model,
                      model_config=ModelConfigProto(name=value['model_name'],
                                                    platform=platform,
                                                    platform_version=platform_version,
                                                    max_batch_size=value['max_batch_size'],
                                                    input=target_model.get_inputs(),
                                                    output=target_model.get_outputs()),
                      path=value['export_path'],
                      version=value.get('version'))

    @staticmethod
    def from_env_and_target_model(env: Mapping[str, str], target_model):
        platform, platform_version = target_model.get_platform()

        return Config(model=target_model,
                      model_config=ModelConfigProto(name=env['MODEL_NAME'],
                                                    platform=platform,
                                                    platform_version=platform_version,
                                                    max_batch_size=int(env['MAX_BATCH_SIZE']),
                                                    input=target_model.get_inputs(),
                                                    output=target_model.get_outputs()),
                      path=env['EXPORT_PATH'],
                      version=utilities.map_optional(env.get('VERSION'), int))


def _get_next_version(model_dir):
    max_version = 0

    for item in os.listdir(model_dir):
        if os.path.isdir(os.path.join(model_dir, item)):
            try:
                max_version = max(max_version, int(item))
            except ValueError:
                pass

    return max_version + 1


def _make_model_package(version_dir, config_file, target_path):
    with ZipFile(file=target_path, mode='w', compression=zipfile.ZIP_DEFLATED) as z_file:
        model_dir_dir = path.dirname(version_dir)

        for root, _, files in os.walk(version_dir):
            for file in files:
                z_file.write(path.join(root, file), path.relpath(path.join(root, file), model_dir_dir))

        z_file.write(config_file, path.basename(config_file))


def save_model(config: Config):
    model_dir = os.path.join(config.path, config.model_config.name)

    # Write `config.pbtxt`.

    os.makedirs(model_dir, exist_ok=True)

    config_file_path = os.path.join(model_dir, 'config.pbtxt')

    with open(config_file_path, 'w') as config_file:
        config_file.write(google.protobuf.text_format.MessageToString(message=config.model_config, as_utf8=True))

    # Normalize version.

    if config.version is None:
        version = _get_next_version(model_dir)
    else:
        version = config.version

    # Save model.

    version_dir = os.path.join(model_dir, str(version))

    config.model.save(version_dir)

    # Make model package.

    package_path = os.path.join(config.path, f'{config.model_config.name}_{version}.zip')

    _make_model_package(version_dir=version_dir, config_file=config_file_path, target_path=package_path)

    return package_path
