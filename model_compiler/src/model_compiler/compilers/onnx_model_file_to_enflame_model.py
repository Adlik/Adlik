# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence

from . import repository
from .. import utilities

import onnx
import onnx.utils

from ..models.data_format import DataFormat
from ..models.sources.onnx_model_file import ONNXModelFile
from ..models.targets.enflame_model import EnflameModel

import TopsInference
from TopsInference import create_parser, create_optimizer

class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':

        return Config(input_formats=utilities.get_data_formats(value.get('input_formats')))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(utilities.split_by(env.get('INPUT_FORMATS'), ',')))
        
@repository.REPOSITORY.register(source_type=ONNXModelFile,
                                target_type=EnflameModel,
                                config_type=Config)
def compile_source(source: ONNXModelFile, config=Config) -> EnflameModel:
    model = onnx.load(source.model_path)
    graph = model.graph
    initializers = {initializer.name for initializer in graph.initializer}
    with TopsInference.device(0, 0) :
        onnx_parser = TopsInference.create_parser(TopsInference.ONNX_MODEL)
        network = onnx_parser.read(source.model_path)
        optimizer = TopsInference.create_optimizer()
        engine = optimizer.build(network)
        return EnflameModel(model_inputs=[input_spec for input_spec in graph.input if input_spec.name not in initializers],
                            model_outputs=graph.output,
                            engine=engine, 
                            input_formats=config.input_formats)
