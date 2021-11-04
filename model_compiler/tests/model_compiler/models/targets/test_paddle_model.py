# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import numpy as np
import paddle
import paddle.optimizer as opt
import tensorflow as tf
from paddle import nn
from paddle.static import InputSpec

from model_compiler.models.data_format import DataFormat
from model_compiler.models.targets.paddle_model import PaddleModel
from model_compiler.protos.generated.model_config_pb2 import (ModelInput,
                                                              ModelOutput)


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


def _make_paddle_model():
    model_dir = TemporaryDirectory()
    get_paddle_model(model_dir.name)
    return PaddleModel.from_directory(model_dir, [DataFormat.CHANNELS_LAST])


class PaddleModelFileTestCase(TestCase):
    @classmethod
    def setUpClass(self):  # pylint: disable=bad-classmethod-argument
        self.paddle_model = _make_paddle_model()

    def test_get_inputs(self):
        self.assertEqual(self.paddle_model.get_inputs(), [
            ModelInput(name='inputs',
                       data_type=tf.float32.as_datatype_enum,
                       format=ModelInput.FORMAT_NHWC,  # pylint: disable=no-member
                       dims=[784])
        ])

    def test_get_outputs(self):
        self.assertEqual(self.paddle_model.get_outputs(), [
            ModelOutput(name='linear_1.tmp_1',
                        data_type=tf.float32.as_datatype_enum,
                        dims=[10])
        ])

    def test_save(self):
        with TemporaryDirectory() as save_path:
            model_save_path = os.path.join(save_path, "saved")
            self.paddle_model.save(model_save_path)
            self.assertEqual(
                sorted(os.listdir(model_save_path)),
                ['model.pdiparams', 'model.pdiparams.info', 'model.pdmodel'])

    def test_get_platform(self):
        platform, version = PaddleModel.get_platform()
        self.assertEqual(platform, 'paddle')
        self.assertNotEqual(version, '')

    def test_from_directory(self):
        model_dir = TemporaryDirectory()
        get_paddle_model(model_dir.name)
        model = PaddleModel.from_directory(model_dir, [DataFormat.CHANNELS_LAST])
        self.assertEqual([input.name for input in model.inputs], ['inputs'])
        self.assertEqual([input.format for input in model.inputs],
                         [ModelInput.FORMAT_NHWC])  # pylint: disable=no-member
