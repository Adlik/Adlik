# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory, NamedTemporaryFile

import onnx
from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.openvino_model import OpenvinoModel
from ..openvino_util import Config, execute_optimize_action


def _get_optimize_params(input_model, output_dir, config):
    params = {'model_name': 'model',
              'input_model': input_model,
              'output_dir': output_dir}
    if config.input_names is not None:
        params['input'] = ','.join(config.input_names)
    if config.input_shapes is None:
        params['batch'] = str(config.max_batch_size)
    else:
        for shape in config.input_shapes:
            shape.insert(0, config.max_batch_size)
        params['input_shape'] = ','.join(str(shape) for shape in config.input_shapes)
    if config.output_names is not None:
        params['output'] = ','.join(config.output_names)
    if config.data_type is not None:
        params['data_type'] = config.data_type
    return params


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: OnnxModel, config: Config) -> OpenvinoModel:
    temp_path = TemporaryDirectory()
    model_path = NamedTemporaryFile(suffix='.onnx')
    onnx.save(source.model_proto, model_path.name)
    optimize_params = _get_optimize_params(model_path.name, temp_path.name, config)
    execute_optimize_action(optimize_params)
    return OpenvinoModel.from_directory(temp_path)
