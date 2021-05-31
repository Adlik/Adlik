# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from unittest import TestCase

import model_compiler.compilers.mxnet_model_file_to_onnx_model as compiler
from model_compiler.compilers.mxnet_model_file_to_onnx_model import Config, DataFormat
from model_compiler.models.sources.mxnet_model_file import MxnetModelFile

import numpy as np
from mxnet import autograd, gluon, nd


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_formats': ['channels_first'],
                                           'input_shapes': [1, 28, 28],
                                           'data_type': 'float32',
                                           'max_batch_size': 1}),
                         Config(input_formats=[DataFormat.CHANNELS_FIRST],
                                input_shapes=[[1, 28, 28]],
                                input_type=np.float32,
                                max_batch_size=1))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_FORMATS': 'channels_last',
                                          'INPUT_SHAPES': '[1, 28, 28]',
                                          'DATA_TYPE': 'S128',
                                          'MAX_BATCH_SIZE': 1}),
                         Config(input_formats=[DataFormat.CHANNELS_LAST],
                                input_shapes=[[1, 28, 28]],
                                input_type=np.dtype('S128'),
                                max_batch_size=1))


def get_mxnet_model(model_path):
    x_label = nd.random.normal(shape=(2, 100))
    y_label = 3 * x_label[:, 0] - 2.5 * x_label[:, 1] + 7.6
    dataset = gluon.data.ArrayDataset(x_label, y_label)
    data_iter = gluon.data.DataLoader(dataset=dataset, batch_size=10, shuffle=True)
    net = gluon.nn.HybridSequential()
    net.add(gluon.nn.PReLU(),
            gluon.nn.Dense(10, activation='relu'),
            gluon.nn.Dense(1))
    net.initialize()
    loss = gluon.loss.L2Loss()
    trainer = gluon.Trainer(net.collect_params(), "sgd", {"learning_rate": 0.03})
    num_epoch = 2
    for _ in range(num_epoch):
        for x_input, y_output in data_iter:
            with autograd.record():
                temp_loss = loss(net(x_input), y_output)
            temp_loss.backward()
            trainer.step(10)
    net.hybridize()
    net(x_label)
    net.export(model_path, num_epoch)


class CompileSourceTestCase(TestCase):
    def test_compile_with_variables(self):
        with TemporaryDirectory() as model_dir:
            get_mxnet_model(model_dir + 'model')
            compiled = compiler.compile_source(MxnetModelFile(model_dir + 'model' + '-' + '0002'),
                                               Config(input_formats=[DataFormat.CHANNELS_LAST],
                                                      input_shapes=[[1, 100]],
                                                      input_type=np.float32,
                                                      max_batch_size=10))
        graph = compiled.model_proto.graph
        initializers = {initializer.name for initializer in graph.initializer}
        input_name = [input_spec.name for input_spec in graph.input if input_spec.name not in initializers]
        self.assertEqual(input_name, ['data'])
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_LAST])