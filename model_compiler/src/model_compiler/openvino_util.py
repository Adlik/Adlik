# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import subprocess  # nosec
from typing import Any, Dict, List, Mapping, NamedTuple, Optional
from openvino.runtime import Core
import openvino

from .models.data_type import DataType
from .protos.generated.model_config_pb2 import ModelInput, ModelOutput
from . import utilities


class ModelParser(NamedTuple):
    model: openvino.pyopenvino.Model

    @staticmethod
    def from_model(model_path: str):
        model = Core().read_model(model_path)
        return ModelParser(model=model)

    def get_inputs(self):
        inputs = []

        for i in range(len(self.model.inputs)):
            inputs.append(ModelInput(name=self.model.inputs[i].get_any_name(),
                                     data_type=DataType.from_openvino_data_type(
                                         self.model.inputs[i].get_element_type().get_type_name()).to_tf_data_type(),
                                     format=ModelInput.FORMAT_NONE,  # pylint: disable=no-member
                                     dims=[-1 if dim is None else dim for dim in
                                           list(self.model.inputs[i].get_shape())[1:]]))
        inputs.sort(key=lambda input_i: input_i.name)
        return inputs

    def get_outputs(self):
        outputs = []
        for i in range(len(self.model.outputs)):
            outputs.append(ModelOutput(name=self.model.outputs[i].get_any_name(),
                                       data_type=DataType.from_openvino_data_type(
                                           self.model.outputs[i].get_element_type().get_type_name()).to_tf_data_type(),
                                       dims=[-1 if dim is None else dim for dim in
                                             list(self.model.outputs[i].get_shape())[1:]]))
        outputs.sort(key=lambda output_i: output_i.name)
        return outputs


class Config(NamedTuple):
    input_names: Optional[List[str]] = None
    input_shapes: Optional[List[list]] = None
    output_names: Optional[List[str]] = None
    data_type: Optional[str] = None
    max_batch_size: Optional[int] = None
    saved_model_tags: Optional[List[str]] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value.get('input_names'),
                      input_shapes=value.get('input_shapes'),
                      output_names=value.get('output_names'),
                      data_type=value.get('data_type'),
                      max_batch_size=value.get('max_batch_size'),
                      saved_model_tags=value.get('saved_model_tags'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_names = utilities.split_by(env.get('INPUT_NAMES'), ',')
        input_shapes = env.get('INPUT_SHAPES')
        input_shapes = None if input_shapes is None else utilities.get_input_shapes_from_env(input_shapes)
        output_names = utilities.split_by(env.get('OUTPUT_NAMES'), ',')
        temp_max_batch_size = env.get('MAX_BATCH_SIZE')
        max_batch_size = int(temp_max_batch_size) if temp_max_batch_size else None
        saved_model_tags = env.get('SAVED_MODEL_TAGS')
        data_type = env.get('DATA_TYPE')

        return Config(input_names=input_names,
                      input_shapes=input_shapes,
                      output_names=output_names,
                      data_type=data_type,
                      max_batch_size=max_batch_size,
                      saved_model_tags=saved_model_tags.split(',') if saved_model_tags else None)


def execute_optimize_action(params: Dict[str, str]):
    subprocess.run(_args_dict_to_list(params), check=True)  # nosec


def _args_dict_to_list(params: Dict[str, str]) -> List[str]:
    args = ['mo']
    for key, value in params.items():
        args.extend(['--' + key] if value is None else ['--' + key, value])
    return args
