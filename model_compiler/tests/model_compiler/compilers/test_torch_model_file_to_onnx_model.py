# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory, NamedTemporaryFile
from unittest import TestCase
import torch
from model_compiler.models.data_format import DataFormat
from model_compiler.compilers.torch_model_file_to_onnx_model import Config
import model_compiler.compilers.torch_model_file_to_onnx_model as compiler
from model_compiler.models.sources.torch_model_file import TorchModelFile


class ConfigTestCase(TestCase):

    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_names': ['input'],
                                           'input_shapes': [1, 2, 3, 4],
                                           'data_type': 'FLOAT16',
                                           'max_batch_size': 1,
                                           'input_formats': ['channels_first']
                                           }),
                         Config(input_names=['input'],
                                input_shapes=[[1, 2, 3, 4]],
                                max_batch_size=1,
                                data_type=torch.float16,
                                input_formats=[DataFormat.CHANNELS_FIRST]))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_NAMES': 'input',
                                          'INPUT_SHAPES': '[1, 2, 3, 4]',
                                          'MAX_BATCH_SIZE': '1',
                                          'DATA_TYPE': 'FLOAT16',
                                          'INPUT_FORMATS': 'channels_last'
                                          }),
                         Config(input_names=['input'],
                                input_shapes=[[1, 2, 3, 4]],
                                max_batch_size=1,
                                data_type=torch.float16,
                                input_formats=[DataFormat.CHANNELS_LAST]))


class Net(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = torch.nn.Sequential(
            torch.nn.Linear(10, 30),
            torch.nn.Linear(30, 5)
            )

    def forward(self, net):
        out = self.linear(net)
        return out


def get_torch_model():
    def weight_init(weight):
        if isinstance(weight, torch.nn.Linear):
            torch.nn.init.xavier_normal_(weight.weight).float()
            torch.nn.init.constant_(weight.bias, 0).float()
    model = Net()
    model.apply(weight_init)
    return model


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with TemporaryDirectory() as model_file:
            model_path = os.path.join(model_file, 'model.pth')
            model = get_torch_model()
            torch.save(model, model_path)
            config = Config(input_names=['fc_w'],
                            input_shapes=[[1, 10]],
                            data_type=torch.float,
                            max_batch_size=2,
                            input_formats=None)

            compiled = compiler.compile_source(TorchModelFile(model_path=model_path), config)
        compiled_graph = compiled.model_proto.graph
        initializers = {initializer.name for initializer in compiled_graph.initializer}
        input_name = [input_spec.name for input_spec in compiled_graph.input if input_spec.name not in initializers]
        self.assertEqual(input_name, ['fc_w'])
        self.assertEqual(compiled.input_data_formats, [None])

    def test_compile_custom_objects(self):
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
                              "        out = self.linear(net)\n"
                              "        return out\n"
                              )
            script_path.seek(0)
            config = Config(input_names=['data'],
                            input_shapes=[[1, 10]],
                            data_type=torch.float,
                            max_batch_size=2,
                            input_formats=[DataFormat.CHANNELS_LAST])
            compiled = compiler.compile_source(TorchModelFile(model_path=model_file.name, script_path=script_path.name),
                                               config)
        compiled_graph = compiled.model_proto.graph
        initializers = {initializer.name for initializer in compiled_graph.initializer}
        input_name = [input_spec.name for input_spec in compiled_graph.input if input_spec.name not in initializers]
        self.assertEqual(input_name, ['data'])
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_LAST])
