# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import copy
from tempfile import TemporaryDirectory
import torch

from . import repository
from .. import utilities
from ..torchscript_util import Config
from ..models.sources.torch_model_file import TorchModelFile
from ..models.targets.torchscript_model import TorchscriptModel


@repository.REPOSITORY.register(source_type=TorchModelFile, target_type=TorchscriptModel,
                                config_type=Config)
def compile_source(source: TorchModelFile, config: Config) -> TorchscriptModel:
    dummy_inputs = []
    input_shapes = copy.deepcopy(config.input_shapes)
    for shape in input_shapes:
        shape.insert(0, config.max_batch_size)
        dummy_inputs.append(torch.ones(shape, dtype=config.data_type))   # pylint: disable=no-member

    if source.script_path:
        model_module = utilities.load_module(source.script_path, 'TheModelClass')
        model = model_module.TheModelClass()
        model.load_state_dict(torch.load(source.model_path, map_location='cpu'))
    else:
        model = torch.load(source.model_path, map_location='cpu')
    temp_path = TemporaryDirectory()
    traced_script_model = torch.jit.trace(model, tuple(dummy_inputs))
    torch.jit.save(traced_script_model, os.path.join(temp_path.name, "model.pth"))
    example_inputs = torch.rand(input_shapes[0])    # pylint: disable=no-member
    model_outputs = traced_script_model(example_inputs)
    return TorchscriptModel(config=config, model_outputs=model_outputs, temp_path=temp_path)
