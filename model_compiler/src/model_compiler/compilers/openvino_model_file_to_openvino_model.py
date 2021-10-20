# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
from glob import glob
from tempfile import TemporaryDirectory

from . import repository
from ..models.sources.openvino_model_file import OpenvinoModelFile
from ..models.targets.openvino_model import OpenvinoModel
from ..openvino_pot_util import Config, execute_pot_action, get_pot_params, get_temp_checker_yaml, \
    get_temp_quantization_json


@repository.REPOSITORY.register(source_type=OpenvinoModelFile, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: OpenvinoModelFile, config: Config) -> OpenvinoModel:
    temp_path = TemporaryDirectory()
    model_path = source.model_path
    if config.enable_int8:
        temp_checker_yaml = get_temp_checker_yaml(config)
        temp_quantization_json = get_temp_quantization_json(config, model_path, temp_checker_yaml)
        pot_params = get_pot_params(temp_path.name, temp_quantization_json.name, config)
        execute_pot_action(pot_params)
        shutil.copy(src=os.path.join(temp_path.name, 'optimized', 'model.xml'), dst=temp_path.name)
        shutil.copy(src=os.path.join(temp_path.name, 'optimized', 'model.bin'), dst=temp_path.name)
        shutil.copy(src=os.path.join(temp_path.name, 'optimized', 'model.mapping'), dst=temp_path.name)
    else:
        shutil.copyfile(src=glob(os.path.join(model_path, '*.xml'))[0], dst=os.path.join(temp_path.name, 'model.xml'))
        shutil.copyfile(src=glob(os.path.join(model_path, '*.bin'))[0], dst=os.path.join(temp_path.name, 'model.bin'))
        shutil.copyfile(src=glob(os.path.join(model_path, '*.mapping'))[0],
                        dst=os.path.join(temp_path.name, 'model.mapping'))
    return OpenvinoModel.from_directory(temp_path)
