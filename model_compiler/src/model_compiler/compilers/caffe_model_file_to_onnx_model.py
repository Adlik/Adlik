# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Any, Mapping, NamedTuple, Optional, Sequence, List
import numpy as np
import caffe2.python.onnx.frontend
from caffe2.proto import caffe2_pb2
from . import repository
from .. import utilities
from ..models.data_format import DataFormat
from ..models.data_type import DataType
from ..models.irs.onnx_model import OnnxModel
from ..models.sources.caffe_model_file import CaffeModelFile


class Config(NamedTuple):
    input_names: Sequence[str]
    input_formats: Sequence[Optional[DataFormat]]
    input_shapes: List[List]
    input_type: np.dtype
    max_batch_size: int

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value['input_names'],
                      input_formats=utilities.get_data_formats(value.get('input_formats')),
                      input_shapes=utilities.get_input_shapes(value['input_shapes']),
                      input_type=DataType.from_caffe_data_type(value['data_type']).to_onnx_data_type(),
                      max_batch_size=value['max_batch_size'])

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_names=env['INPUT_NAMES'].split(','),
                      input_formats=utilities.get_data_formats(utilities.split_by(env.get('INPUT_FORMATS'), ',')),
                      input_shapes=utilities.get_input_shapes(
                          utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES'))
                      ),
                      input_type=DataType.from_caffe_data_type(env['DATA_TYPE']).to_onnx_data_type(),
                      max_batch_size=int(env['MAX_BATCH_SIZE']))


def parse_caffe_net(net, pb_path):
    with open(pb_path, 'rb') as file:
        net.ParseFromString(file.read())
    return net


@repository.REPOSITORY.register(source_type=CaffeModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: CaffeModelFile, config: Config) -> OnnxModel:
    predict_net = parse_caffe_net(caffe2_pb2.NetDef(), os.path.join(source.model_path, 'predict_net.pb'))
    predict_net.name = "model" if predict_net.name == "" else predict_net.name  # pylint: disable=no-member
    init_net = parse_caffe_net(caffe2_pb2.NetDef(), os.path.join(source.model_path, 'init_net.pb'))

    value_info = {}
    for i, input_shape in enumerate(config.input_shapes):
        input_shape.insert(0, config.max_batch_size)
        value_info[config.input_names[i]] = (config.input_type, input_shape)

    onnx_model = caffe2.python.onnx.frontend.caffe2_net_to_onnx_model(predict_net, init_net, value_info)

    graph = onnx_model.graph  # pylint: disable=no-member
    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph,
                                                                                    config.input_formats))
