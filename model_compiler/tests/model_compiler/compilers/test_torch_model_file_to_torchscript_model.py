# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import NamedTemporaryFile
from unittest import TestCase
from tensorflow.core.framework.types_pb2 import DataType    # pylint: disable=no-name-in-module
import torch

from model_compiler.torchscript_util import Config
import model_compiler.compilers.torch_model_file_to_torchscript_model as compiler
from model_compiler.models.sources.torch_model_file import TorchModelFile
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


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


class CompileSourceTestCase(TestCase):
    def test_compile(self):
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
                                       'data_type': torch.float,   # pylint: disable=no-member
                                       'max_batch_size': 1,
                                       'input_formats': 'channels_last'})
            compiled = compiler.compile_source(TorchModelFile(model_path=model_file.name, script_path=script_path.name),
                                               config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='data', data_type=DataType.DT_FLOAT,
                                     format=ModelInput.FORMAT_NHWC, dims=[1, 10])])   # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='output_0', data_type=DataType.DT_FLOAT,   # pylint: disable=no-member
                                      dims=[1, 5])])
