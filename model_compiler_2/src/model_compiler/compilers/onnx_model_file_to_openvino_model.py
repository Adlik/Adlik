# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory

from . import repository
from ..models.sources.onnx_model_file import ONNXModelFile
from ..models.targets.openvino_model import OpenvinoModel
from ..openvino_util import Config, ModelParser, execute_optimize_action


def _get_optimize_params(input_model, output_dir, config):
    params = {'script_name': 'mo_onnx.py',
              'model_name': 'model',
              'input_model': input_model,
              'output_dir': output_dir}
    if config.input_names is not None:
        params['input'] = config.input_names
    if config.input_shapes is not None:
        params['input_shape'] = config.input_shapes
    if config.output_names is not None:
        params['output'] = config.output_names
    if config.max_batch_size is not None:
        params['batch'] = str(config.max_batch_size)
    return params


@repository.REPOSITORY.register(source_type=ONNXModelFile, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: ONNXModelFile, config: Config) -> OpenvinoModel:
    temp_path = TemporaryDirectory()
    optimize_params = _get_optimize_params(source.model_path, temp_path.name, config)
    execute_optimize_action(optimize_params)
    model_parser = ModelParser.from_xml(os.path.join(temp_path.name, 'model.xml'))
    inputs = model_parser.get_inputs()
    outputs = model_parser.get_outputs()
    return OpenvinoModel(inputs, outputs, temp_path)
