# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase
import tensorflow as tf
from tf2onnx import tfonnx, tf_loader
import onnx
from model_compiler.models.targets.onnx_model import DataFormat, ONNXModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_onnx_model():
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        tf.multiply(input_x + input_y, weight, name='z')
        session.run(weight.initializer)
        frozen_graph_def = tf_loader.freeze_session(session, input_names=['x:0', 'y:0'], output_names=['z:0'])

    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        tf.import_graph_def(frozen_graph_def, name='')
        onnx_model = tfonnx.process_tf_graph(tf_graph=session.graph,
                                             input_names=['x:0', 'y:0'],
                                             output_names=['z:0']).make_model(graph_doc='Test onnx model')

    return ONNXModel(onnx_model=onnx_model,
                     input_formats=[DataFormat.CHANNELS_LAST, DataFormat.CHANNELS_FIRST],
                     model_inputs=onnx_model.graph.input)


class OnnxModelFileTestCase(TestCase):
    def test_get_single_inputs(self):
        onnx_model = _make_onnx_model()

        self.assertEqual(onnx_model.get_inputs(), [ModelInput(name='x:0', data_type='DT_FLOAT',
                                                              format='FORMAT_NHWC', dims=[2, 3, 4]),
                                                   ModelInput(name='y:0', data_type='DT_FLOAT',
                                                              format='FORMAT_NCHW', dims=[2, 3, 4])])

    def test_get_outputs(self):
        onnx_model = _make_onnx_model()

        self.assertEqual(onnx_model.get_outputs(), [ModelOutput(name='z:0', data_type='DT_FLOAT', dims=[2, 3, 4])])

    def test_save(self):
        onnx_model = _make_onnx_model()

        with TemporaryDirectory() as save_path:
            onnx_model.save(save_path)

            self.assertEqual(os.listdir(save_path), ['model.onnx'])

    def test_get_platform(self):
        self.assertEqual(ONNXModel.get_platform(), ('onnx', onnx.version.version))
