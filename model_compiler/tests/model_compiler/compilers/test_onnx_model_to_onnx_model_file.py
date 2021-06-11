# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase
import tensorflow as tf
from tf2onnx import tfonnx, tf_loader
from onnx import ModelProto
from model_compiler.models.irs.onnx_model import DataFormat, OnnxModel
import model_compiler.compilers.onnx_model_to_onnx_model_file as compiler


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
    return OnnxModel(model_proto=onnx_model,
                     input_data_formats=[DataFormat.CHANNELS_LAST, DataFormat.CHANNELS_FIRST])


class OnnxModelFileTestCase(TestCase):
    def test_compile_with_formats(self):
        onnx_model = _make_onnx_model()
        compiled = compiler.compile_source(source=onnx_model)
        self.assertIsInstance(compiled.onnx_model, ModelProto)
        self.assertEqual(compiled.input_formats, [DataFormat.CHANNELS_LAST, DataFormat.CHANNELS_FIRST])
        self.assertEqual([model_input.name for model_input in compiled.model_inputs], ['x:0', 'y:0'])
