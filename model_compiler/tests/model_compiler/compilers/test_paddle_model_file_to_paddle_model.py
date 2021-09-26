# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from unittest import TestCase
import os
import numpy as np
import paddle
from paddle import nn
import paddle.optimizer as opt
from paddle.static import InputSpec

import model_compiler.compilers.paddle_model_file_to_paddle_model as compiler
from model_compiler.compilers.paddle_model_file_to_paddle_model import Config, DataFormat
from model_compiler.protos.generated.model_config_pb2 import ModelInput
from model_compiler.models.sources.paddle_model_file import PaddlePaddleModelFile


class _RandomDataset(paddle.io.Dataset):
    def __init__(self, num_samples):  # pylint: disable=super-init-not-called
        self.num_samples = num_samples

    def __getitem__(self, idx):
        image = np.random.random([784]).astype('float32')  # pylint: disable=no-member
        label = np.random.randint(0, 9, (1, )).astype('int64')
        return image, label

    def __len__(self):
        return self.num_samples


class _LinearNet(nn.Layer):
    def __init__(self):
        super().__init__()
        self._linear = nn.Linear(784, 10)

    def forward(self, inputs):  # pylint: disable=arguments-differ
        return self._linear(inputs)


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(
            Config.from_json({
                'input_formats': ['channels_first'],
                'model_filename': 'model',
                'params_filename': 'params'
            }),
            Config(input_formats=[DataFormat.CHANNELS_FIRST],
                   model_filename='model',
                   params_filename='params'))

    def test_from_env(self):
        self.assertEqual(
            Config.from_env({
                'INPUT_FORMATS': 'channels_last',
                'MODEL_FILENAME': None,
                'PARAMS_FILENAME': None
            }),
            Config(input_formats=[DataFormat.CHANNELS_LAST],
                   model_filename=None,
                   params_filename=None))


def get_paddle_model(model_path):
    def train(layer, loader, loss_fn, optimizer):
        for _ in range(1):
            for _, (image, label) in enumerate(loader()):
                out = layer(image)
                loss = loss_fn(out, label)
                loss.backward()
                optimizer.step()
                optimizer.clear_grad()

    paddle.disable_static()
    model_layer = _LinearNet()
    loss_func = nn.CrossEntropyLoss()
    adam = opt.Adam(learning_rate=0.001, parameters=model_layer.parameters())

    dataset = _RandomDataset(64)
    data_loader = paddle.io.DataLoader(dataset,
                                       batch_size=16,
                                       shuffle=True,
                                       drop_last=True,
                                       num_workers=2)

    train(model_layer, data_loader, loss_func, adam)
    paddle.jit.save(layer=model_layer,
                    path=os.path.join(model_path, 'model'),
                    input_spec=[InputSpec(shape=[None, 784], dtype='float32')])


class CompileSourceTestCase(TestCase):
    def test_compile(self):
        with TemporaryDirectory() as model_dir:
            get_paddle_model(model_dir)
            compiled = compiler.compile_source(
                PaddlePaddleModelFile(model_dir),
                Config(input_formats=[DataFormat.CHANNELS_LAST],
                       model_filename=os.path.join(model_dir, 'model.pdmodel'),
                       params_filename=os.path.join(model_dir,
                                                    'model.pdiparams')))
        self.assertEqual([input.name for input in compiled.inputs], ['inputs'])
        self.assertEqual([input.format for input in compiled.inputs],
                         [ModelInput.FORMAT_NHWC])  # pylint: disable=no-member
