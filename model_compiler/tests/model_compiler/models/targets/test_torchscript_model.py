# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase
from tensorflow.core.framework.types_pb2 import DataType   # pylint: disable=no-name-in-module
import torch

import model_compiler.compilers.torch_model_file_to_torchscript_model as compiler
from model_compiler.torchscript_util import Config
from model_compiler.models.targets.torchscript_model import TorchscriptModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput
from model_compiler.models.sources.torch_model_file import TorchModelFile


class _Net(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = torch.nn.Sequential(
            torch.nn.Linear(10, 30),
            torch.nn.Linear(30, 5)
            )

    def forward(self, net):
        return self.linear(net)


def get_torch_model():
    def weight_init(weight):
        if isinstance(weight, torch.nn.Linear):
            torch.nn.init.xavier_normal_(weight.weight).float()
            torch.nn.init.constant_(weight.bias, 0).float()
    model = _Net()
    model.apply(weight_init)
    return model


def _make_torchscript_model() -> TorchscriptModel:
    with NamedTemporaryFile(mode='w+', suffix='.py') as script_path, \
            NamedTemporaryFile(suffix='.pth') as model_file:
        model = get_torch_model()
        torch.save(model.state_dict(), model_file)
        script_path.write("import torch\n"
                          "\n\n"
                          "class TheModelClass(torch.nn.Module):\n"
                          "    def __init__(self):\n"
                          "        super().__init__()\n"
                          "        self.linear = torch.nn.Sequential(\n"
                          "            torch.nn.Linear(10, 30),\n"
                          "            torch.nn.Linear(30, 5))\n\n"
                          "    def forward(self, net):\n"
                          "        return self.linear(net)\n"
                          )
        script_path.seek(0)

        config = Config.from_json({'input_names': ['data'],
                                   'input_shapes': [1, 10],
                                   'data_type': 'FLOAT',
                                   'max_batch_size': 1,
                                   'input_formats': 'channels_last'})
        compiled = compiler.compile_source(TorchModelFile(model_path=model_file.name, script_path=script_path.name),
                                           config)

    return compiled


class TorchscriptModelTestCase(TestCase):
    def test_get_inputs(self):
        model = _make_torchscript_model()
        self.assertEqual(model.get_inputs(),
                         [ModelInput(name='data', data_type=DataType.DT_FLOAT,
                                     format=ModelInput.FORMAT_NHWC, dims=[1, 10])])    # pylint: disable=no-member

    def test_get_outputs(self):
        model = _make_torchscript_model()
        self.assertEqual(model.get_outputs(),
                         [ModelOutput(name='output_0', data_type=DataType.DT_FLOAT,    # pylint: disable=no-member
                                      dims=[1, 5])])

    def test_save(self):
        model = _make_torchscript_model()
        with TemporaryDirectory() as save_path:
            model.save(save_path)
            self.assertEqual(os.listdir(save_path), ['model.pth'])

    def test_get_platform(self):
        self.assertEqual(TorchscriptModel.get_platform(), ('libtorch', torch.__version__))
