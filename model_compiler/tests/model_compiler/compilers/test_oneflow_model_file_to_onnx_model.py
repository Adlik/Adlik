# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=no-member

from tempfile import NamedTemporaryFile
from unittest import TestCase
import oneflow
from model_compiler.models.data_format import DataFormat
from model_compiler.compilers.oneflow_model_file_to_onnx_model import Config
import model_compiler.compilers.oneflow_model_file_to_onnx_model as compiler
from model_compiler.models.sources.oneflow_model_file import OneFlowModelFile


class ConfigTestCase(TestCase):

    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_shapes': [1, 2, 3, 4],
                                           'data_type': 'FLOAT',
                                           'max_batch_size': 1,
                                           'input_formats': ['channels_first']
                                           }),
                         Config(input_shapes=[[1, 2, 3, 4]],
                                data_type=oneflow.float,
                                max_batch_size=1,
                                input_formats=[DataFormat.CHANNELS_FIRST]))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_SHAPES': '[1, 2, 3, 4]',
                                          'DATA_TYPE': 'FLOAT',
                                          'MAX_BATCH_SIZE': '1',
                                          'INPUT_FORMATS': 'channels_last'
                                          }),
                         Config(input_shapes=[[1, 2, 3, 4]],
                                data_type=oneflow.float,
                                max_batch_size=1,
                                input_formats=[DataFormat.CHANNELS_LAST]))


class _Net(oneflow.nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = oneflow.nn.Sequential(
            oneflow.nn.Linear(10, 30),
            oneflow.nn.Linear(30, 5)
            )

    def forward(self, *args, **kwargs):
        return self.linear(*args, **kwargs)


def get_oneflow_model():
    def weight_init(weight):
        if isinstance(weight, oneflow.nn.Linear):
            oneflow.nn.init.xavier_normal_(weight.weight).float()
            oneflow.nn.init.constant_(weight.bias, 0).float()
    model = _Net()
    model.apply(weight_init)
    return model


class CompileSourceTestCase(TestCase):

    def test_compile(self):
        with NamedTemporaryFile('w+', suffix='.py') as script_file:
            model = get_oneflow_model()
            oneflow.save(model.state_dict(), 'model')
            script_file.write("import oneflow\n"
                              "\n\n"
                              "class Model(oneflow.nn.Module):\n"
                              "    def __init__(self):\n"
                              "        super().__init__()\n"
                              "        self.linear = oneflow.nn.Sequential(\n"
                              "            oneflow.nn.Linear(10, 30),\n"
                              "            oneflow.nn.Linear(30, 5))\n\n"
                              "    def forward(self, *args, **kwargs):\n"
                              "        return self.linear(*args, **kwargs)\n"
                              )
            script_file.seek(0)

            config = Config(input_shapes=[[10]],
                            data_type=oneflow.float,
                            max_batch_size=2,
                            input_formats=[DataFormat.CHANNELS_LAST])
            compiled = compiler.compile_source(OneFlowModelFile(model_path='model', script_path=script_file.name),
                                               config)
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_LAST])
