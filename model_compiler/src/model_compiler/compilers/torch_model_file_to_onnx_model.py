# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, List, Sequence
from tempfile import NamedTemporaryFile
import importlib.util
import torch
import onnx


from . import repository
from .. import utilities
from ..models.sources.torch_model_file import TorchModelFile
from ..models.irs.onnx_model import OnnxModel
from ..models.data_type import DataType
from ..models.data_format import DataFormat
from ..models import data_format


class Config(NamedTuple):
    input_names: Sequence[str]
    input_shapes: List[List]
    data_type: torch.dtype    # pylint: disable=no-member
    max_batch_size: int
    input_formats: Optional[Sequence[Optional[DataFormat]]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        raw_input_formats: Optional[str] = value.get('input_formats')

        return Config(input_names=value['input_names'],
                      input_shapes=utilities.get_input_shapes(value.get('input_shapes')),
                      data_type=DataType.from_torch_data_type(value['data_type']),
                      max_batch_size=value['max_batch_size'],
                      input_formats=utilities.map_optional(
                          raw_input_formats,
                          lambda formats: list(map(data_format.str_to_data_format, formats))))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_shapes = utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES'))
        data_type = DataType.from_torch_data_type(env.get('DATA_TYPE'))

        return Config(input_names=env['INPUT_NAMES'].split(','),
                      input_shapes=input_shapes,
                      data_type=data_type,
                      max_batch_size=int(env['MAX_BATCH_SIZE']),
                      input_formats=utilities.map_optional(
                          utilities.split_by(env.get('INPUT_FORMATS'), ','),
                          lambda formats: list(map(data_format.str_to_data_format, formats))))


def _load_module(file_path, name):
    spec = importlib.util.spec_from_file_location(name=name, location=file_path)
    module = importlib.util.module_from_spec(spec=spec)
    spec.loader.exec_module(module)
    return module


@repository.REPOSITORY.register(source_type=TorchModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: TorchModelFile, config: Config) -> OnnxModel:
    dummy_inputs = []
    for shape in config.input_shapes:
        shape.insert(0, config.max_batch_size)
        dummy_inputs.append(torch.ones(shape, dtype=config.data_type))    # pylint: disable=no-member

    if source.script_path:
        model_module = _load_module(source.script_path, 'TheModelClass')
        model = model_module.TheModelClass()
        model.load_state_dict(torch.load(source.model_path))
    else:
        model = torch.load(source.model_path)

    with NamedTemporaryFile(suffix='.onnx') as onnx_path:
        torch.onnx.export(model,
                          tuple(dummy_inputs),
                          onnx_path.name,
                          verbose=True,
                          input_names=config.input_names)
        onnx_model = onnx.load(onnx_path.name)

    onnx.checker.check_model(onnx_model)
    graph = onnx_model.graph  # pylint: disable=no-member
    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, config.input_formats))
