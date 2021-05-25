# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, List, Sequence
import importlib.util
from tempfile import NamedTemporaryFile
import torch
import onnx
from ..models.sources.torch_model_file import TorchModelFile
from ..models.irs.onnx_model import OnnxModel
from . import repository
from .. import utilities
from ..models.data_format import DataFormat
from ..models import data_format


def get_torch_data_type_form_str(type_str):
    torch_data_type_map = {
        'FLOAT': torch.float,
        'DOUBLE': torch.double,
        'COMPLEX64': torch.complex64,
        'COMPLEX128': torch.complex128,
        'FLOAT16': torch.float16,
        'BFLOAT16': torch.bfloat16,
        'UINT8': torch.uint8,
        'INT8': torch.int8,
        'INT16': torch.int16,
        'INT32': torch.int32,
        'INT64': torch.int64,
        'BOOL': torch.bool
    }
    return torch_data_type_map[type_str.upper()]


class Config(NamedTuple):
    input_names: Sequence[str]
    input_shapes: List[List]
    data_type: torch.dtype
    max_batch_size: int
    input_formats: Optional[Sequence[Optional[DataFormat]]]

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        raw_input_formats: Optional[str] = value.get('input_formats')
        return Config(input_names=value['input_names'],
                      input_shapes=utilities.get_input_shapes(value.get('input_shapes')),
                      data_type=get_torch_data_type_form_str(value['data_type']),
                      max_batch_size=value['max_batch_size'],
                      input_formats=utilities.map_optional(
                          raw_input_formats,
                          lambda formats: list(map(data_format.str_to_data_format, formats))))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_shapes = utilities.get_input_shapes_from_env(env.get('INPUT_SHAPES'))
        data_type = get_torch_data_type_form_str(env.get('DATA_TYPE'))
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
    input_shapes = config.input_shapes
    data_type = config.data_type
    max_batch_size = config.max_batch_size
    input_formats = config.input_formats
    dummy_inputs = []
    onnx_path = NamedTemporaryFile(suffix='.onnx')
    for shape in input_shapes:
        shape.insert(0, max_batch_size)
        dummy_inputs.append(torch.ones(shape, dtype=data_type))
    if source.script_path:
        model_module = _load_module(source.script_path, 'TheModelClass')
        model = model_module.TheModelClass()
        model.load_state_dict(torch.load(source.model_path))

    else:
        model = torch.load(source.model_path)
    torch.onnx.export(model,
                      tuple(dummy_inputs),
                      onnx_path.name,
                      verbose=True,
                      input_names=config.input_names)
    onnx_model = onnx.load(onnx_path.name)
    onnx.checker.check_model(onnx_model)
    graph = onnx_model.graph  # pylint: disable=no-member
    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=utilities.get_onnx_model_input_data_formats(graph, input_formats))
