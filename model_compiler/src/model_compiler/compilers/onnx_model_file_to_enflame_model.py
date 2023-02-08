# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, List, Sequence

import onnx
import onnx.utils
import json

from . import repository
from .. import utilities

from ..models.data_format import DataFormat
from ..models.sources.onnx_model_file import ONNXModelFile
from ..models.targets.enflame_model import EnflameModel


class Config(NamedTuple):
    input_formats: Sequence[Optional[DataFormat]]
    resource_mode: Optional[str]
    build_flag: Optional[str]
    input_shapes: List[List]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(
            value.get('input_formats')),
                      input_shapes=value.get('input_shapes'),
                      resource_mode=value.get('resource_mode'),
                      build_flag=value.get('build_flag'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_formats=utilities.get_data_formats(
            utilities.split_by(env.get('INPUT_FORMATS'), ',')),
                      input_shapes=env.get('INPUT_SHAPES'),
                      resource_mode=env.get('RESOURCE_MODE'),
                      build_flag=env.get('BUILD_FLAG'))


@repository.REPOSITORY.register(source_type=ONNXModelFile,
                                target_type=EnflameModel,
                                config_type=Config)
def compile_source(source: ONNXModelFile, config=Config) -> EnflameModel:
    import TopsInference  # pylint: disable=import-outside-toplevel,import-error

    graph = onnx.load(source.model_path).graph  # pylint: disable=no-member
    initializers = {initializer.name for initializer in graph.initializer}
    with TopsInference.device(0, 0):
        onnx_parser = TopsInference.create_parser(TopsInference.ONNX_MODEL)
        if config.input_shapes:
            onnx_parser.set_input_shapes(config.input_shapes)
        network = onnx_parser.read(source.model_path)
        compile_options = {'resource_mode': config.resource_mode}
        optimizer = TopsInference.create_optimizer()
        optimizer.set_compile_options(json.dumps(compile_options))
        if config.build_flag == 'TTopsInference.KFP16_MIX':
            optimizer.set_build_flag(TopsInference.KFP16_MIX)
        if config.build_flag == 'TopsInference.KFP16':
            optimizer.set_build_flag(TopsInference.KFP16)
        if config.build_flag == 'TopsInference.KDEFAULT':
            optimizer.set_build_flag(TopsInference.KDEFAULT)
        engine = optimizer.build(network)
        return EnflameModel(model_inputs=[
            input_spec for input_spec in graph.input
            if input_spec.name not in initializers
        ],
                            model_outputs=graph.output,
                            engine=engine,
                            input_formats=config.input_formats)
