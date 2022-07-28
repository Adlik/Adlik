# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf
from tf2onnx import tfonnx, tf_loader

import model_compiler.compilers.onnx_model_to_openvino_model as compiler
from model_compiler.compilers.onnx_model_to_openvino_model import Config
from model_compiler.models.irs.onnx_model import OnnxModel
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
        return onnx_model


def _make_onnx_model_fp16():
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float16, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float16, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float16)
        tf.multiply(input_x + input_y, weight, name='z')
        session.run(weight.initializer)
        frozen_graph_def = tf_loader.freeze_session(session, input_names=['x:0', 'y:0'], output_names=['z:0'])

    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        tf.import_graph_def(frozen_graph_def, name='')
        onnx_model = tfonnx.process_tf_graph(tf_graph=session.graph,
                                             input_names=['x:0', 'y:0'],
                                             output_names=['z:0']).make_model(graph_doc='Test onnx model')
        return onnx_model


class CompileSourceTestCase(TestCase):
    def test_compile_with_no_params(self):
        onnx_model = _make_onnx_model()
        config = Config.from_json({'max_batch_size': 1})
        compiled = compiler.compile_source(OnnxModel(model_proto=onnx_model, input_data_formats=[None, None]), config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x:0', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                          ModelInput(name='y:0', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member

        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z:0', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_all_params(self):
        onnx_model = _make_onnx_model()
        config = Config.from_json({'input_names': ['0:add', '1:add'],
                                   'input_shapes': [[2, 3, 4], [2, 3, 4]],
                                   'output_names': ['z'],
                                   'max_batch_size': 1})
        compiled = compiler.compile_source(OnnxModel(model_proto=onnx_model, input_data_formats=[None, None]), config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x:0', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                          ModelInput(name='y:0', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z_raw_output___17:0',
                                      data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_fp16(self):
        onnx_model = _make_onnx_model_fp16()
        config = Config.from_json({'max_batch_size': 1,
                                   'data_type': 'FP16'})
        compiled = compiler.compile_source(OnnxModel(model_proto=onnx_model, input_data_formats=[None, None]), config)

        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x:0', data_type=tf.float16.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                          ModelInput(name='y:0', data_type=tf.float16.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member

        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z:0', data_type=tf.float16.as_datatype_enum, dims=[2, 3, 4])])
