# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from unittest import TestCase
import os
import numpy as np
from caffe2.python import workspace, model_helper
from caffe2.python.predictor import mobile_exporter

import model_compiler.compilers.caffe_model_file_to_onnx_model as compiler
from model_compiler.compilers.caffe_model_file_to_onnx_model import Config, DataFormat
from model_compiler.models.sources.caffe_model_file import CaffeModelFile


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_names': ['input'],
                                           'input_formats': ['channels_first'],
                                           'input_shapes': [1, 28, 28],
                                           'data_type': 'float',
                                           'max_batch_size': 1}),
                         Config(input_names=['input'],
                                input_formats=[DataFormat.CHANNELS_FIRST],
                                input_shapes=[[1, 28, 28]],
                                input_type=1,
                                max_batch_size=1))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_NAMES': 'input',
                                          'INPUT_FORMATS': 'channels_last',
                                          'INPUT_SHAPES': '[1, 28, 28]',
                                          'DATA_TYPE': 'float',
                                          'MAX_BATCH_SIZE': 1}),
                         Config(input_names=['input'],
                                input_formats=[DataFormat.CHANNELS_LAST],
                                input_shapes=[[1, 28, 28]],
                                input_type=1,
                                max_batch_size=1))


def get_caffe_model(model_path):
    def _save_caffe_net(net, pb_path):
        with open(pb_path, 'wb') as file:
            file.write(net.SerializeToString())

    data = np.random.rand(16, 100).astype(np.float32)
    workspace.FeedBlob('data', data)

    deploy_model = model_helper.ModelHelper(name='')
    deploy_model.param_init_net.XavierFill([], 'fc_w', shape=[16, 100])
    deploy_model.param_init_net.ConstantFill([], 'fc_b', shape=[16, ])
    fc_1 = deploy_model.net.FC(['data', 'fc_w', 'fc_b'], 'fc1')
    deploy_model.net.Sigmoid(fc_1, 'pred')

    workspace.RunNetOnce(deploy_model.param_init_net)
    workspace.CreateNet(deploy_model.net)

    init_net, predict_net = mobile_exporter.Export(workspace, deploy_model.net, deploy_model.params)

    _save_caffe_net(init_net, os.path.join(model_path, 'init_net.pb'))
    _save_caffe_net(predict_net, os.path.join(model_path, 'predict_net.pb'))


class CompileSourceTestCase(TestCase):
    def test_compile(self):
        with TemporaryDirectory() as model_dir:
            get_caffe_model(model_dir)
            compiled = compiler.compile_source(CaffeModelFile(model_dir),
                                               Config(input_names=['data', 'fc_w', 'fc_b'],
                                                      input_formats=[DataFormat.CHANNELS_LAST, None, None],
                                                      input_shapes=[[100], [100], []],
                                                      input_type=1,
                                                      max_batch_size=16))
        graph = compiled.model_proto.graph
        initializers = {initializer.name for initializer in graph.initializer}
        input_name = [input_spec.name for input_spec in graph.input if input_spec.name not in initializers]
        self.assertEqual(input_name, ['data', 'fc_w', 'fc_b'])
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_LAST, None, None])
