# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=no-member

from typing import Any, Mapping, NamedTuple, Optional, List, Sequence
from tempfile import TemporaryDirectory
import os
import oneflow as flow
import oneflow.nn as nn
from oneflow_onnx.oneflow2onnx.util import convert_to_onnx_and_check
import onnx

from . import repository
from .. import utilities
from ..models.sources.oneflow_model_file import OneFlowModelFile
from ..models.irs.onnx_model import OnnxModel
from ..models.data_format import DataFormat
from ..models.data_type import DataType


class Config(NamedTuple):
    input_shapes: List[List]
    data_type: flow.dtype
    max_batch_size: int
    input_formats: Sequence[Optional[DataFormat]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_shapes=utilities.get_input_shapes(value.get('input_shapes')),
                      data_type=DataType.from_oneflow_data_type(value['data_type']),
                      max_batch_size=value['max_batch_size'],
                      input_formats=utilities.get_data_formats(value.get('input_formats')))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_shapes=utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES')),
                      data_type=DataType.from_oneflow_data_type(env.get('DATA_TYPE')),
                      max_batch_size=int(env['MAX_BATCH_SIZE']),
                      input_formats=utilities.get_data_formats(utilities.split_by(env.get('INPUT_FORMATS'), ',')))


@repository.REPOSITORY.register(source_type=OneFlowModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: OneFlowModelFile, config: Config) -> OnnxModel:
    class Graph(nn.Graph):
        def __init__(self, eager_model):
            super().__init__()
            self.model = eager_model

        def build(self, *args, **kwargs):
            return self.model(*args, **kwargs)

    dummy_inputs = []
    for shape in config.input_shapes:
        shape.insert(0, config.max_batch_size)
        dummy_inputs.append(flow.ones(shape, dtype=config.data_type))

    model_module = utilities.load_module(source.script_path, 'Model')
    model = model_module.Model()
    model.load_state_dict(flow.load(source.model_path))
    model.eval()
    model_graph = Graph(model)
    model_graph(*dummy_inputs)

    with TemporaryDirectory() as tmpdirname, TemporaryDirectory() as modeldirname:
        flow.save(model.state_dict(), tmpdirname)
        convert_to_onnx_and_check(model_graph,
                                  flow_weight_dir=tmpdirname,
                                  onnx_model_path=modeldirname,
                                  print_outlier=False)
        onnx_model = onnx.load(os.path.join(modeldirname, 'model.onnx'))

    onnx.checker.check_model(onnx_model)
    graph = onnx_model.graph
    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, config.input_formats))
