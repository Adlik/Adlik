# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence, List
from tempfile import TemporaryDirectory, NamedTemporaryFile

import mxnet as mx
import numpy as np
import onnx

from mxnet.contrib import onnx as onnx_mxnet

from . import repository
from .. import utilities
from ..models import data_format
from ..models.data_format import DataFormat
from ..models.irs.onnx_model import OnnxModel
from ..models.sources.mxnet_model_file import MxnetModelFile


def get_mxnet_data_type(data_type) -> 'np.dtype':
    if data_type.upper() in ['S128', 'S64', 'S32', 'S16', 'S8']:
        data_type = np.dtype(data_type.upper())
    else:
        data_type = np.dtype(data_type.lower())
    return data_type


class Config(NamedTuple):
    input_formats: Optional[Sequence[Optional[DataFormat]]]
    input_shapes: List[List]
    input_type: np.dtype
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        raw_input_formats: Optional[str] = value.get('input_formats')

        return Config(input_formats=utilities.map_optional(raw_input_formats,
                      lambda formats: list(map(data_format.str_to_data_format, formats))),
                      input_shapes=utilities.get_input_shapes(value['input_shapes']),
                      input_type=get_mxnet_data_type(value['data_type']),
                      max_batch_size=value['max_batch_size'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.map_optional(utilities.split_by(env.get('INPUT_FORMATS'), ','),
                      lambda formats: list(map(data_format.str_to_data_format, formats))),
                      input_shapes=utilities.get_input_shapes(
                          utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES'))
                      ),
                      input_type=get_mxnet_data_type(env['DATA_TYPE']),
                      max_batch_size=int(env['MAX_BATCH_SIZE'])
                      )


@repository.REPOSITORY.register(source_type=MxnetModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: MxnetModelFile, config: Config) -> OnnxModel:
    num_epoch = int(source.model_path.rpartition('-')[-1])

    sym, arg_params, aux_params = mx.model.load_checkpoint(source.model_path.rpartition('-')[0], num_epoch)
    new_arg_params = {}
    for operation, value in arg_params.items():
        if 'relu' in operation:
            value = value.reshape(1, -1, 1, 1)
        new_arg_params[operation] = value

    for input_shape in config.input_shapes:
        input_shape.insert(0, config.max_batch_size)

    with TemporaryDirectory() as new_model_dir:
        new_model_path = new_model_dir + 'model'
        mx.model.save_checkpoint(new_model_path, num_epoch, sym, new_arg_params, aux_params)

        with NamedTemporaryFile(suffix='.onnx') as model_file:
            onnx_mxnet.export_model(sym=new_model_path + '-symbol.json',
                                    params=new_model_path + '-{:04d}.params'.format(num_epoch),
                                    input_shape=config.input_shapes,
                                    input_type=config.input_type, onnx_file_path=model_file.name)
            onnx_model = onnx.load(model_file.name)

    onnx.checker.check_model(onnx_model)

    graph = onnx_model.graph  # pylint: disable=no-member

    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, config.input_formats))