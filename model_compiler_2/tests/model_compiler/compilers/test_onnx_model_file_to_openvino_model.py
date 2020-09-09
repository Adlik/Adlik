# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import NamedTemporaryFile
from unittest import TestCase

import tensorflow as tf
from tf2onnx import tfonnx

import model_compiler.compilers.onnx_model_file_to_openvino_model as compiler
from model_compiler.compilers.onnx_model_file_to_openvino_model import Config
from model_compiler.models.sources.onnx_model_file import ONNXModelFile
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _save_onnx_model(model_file):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        tf.multiply(input_x + input_y, weight, name='z')
        session.run(weight.initializer)

        constant_graph = tf.compat.v1.graph_util.convert_variables_to_constants(session, session.graph_def, ['z'])

    with tf.Graph().as_default() as graph, open(model_file.name, mode='wb') as graph_file:
        tf.graph_util.import_graph_def(graph_def=constant_graph)
        onnx_model = tfonnx.process_tf_graph(tf_graph=graph).make_model(graph_doc='Test onnx model')
        graph_file.write(onnx_model.SerializeToString())


class CompileSourceTestCase(TestCase):
    def test_compile_with_no_params(self):
        with NamedTemporaryFile(suffix='.onnx') as model_file:
            _save_onnx_model(model_file)
            config = Config.from_json({'max_batch_size': 1})
            compiled = compiler.compile_source(ONNXModelFile(model_path=model_file.name), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='import/x:0', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='import/y:0', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='import/z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_all_params(self):
        with NamedTemporaryFile(suffix='.onnx') as model_file:
            _save_onnx_model(model_file)
            config = Config.from_json({'input_names': ['0:import/add', '1:import/add'],
                                       'input_shapes': [[1, 2, 3, 4], [1, 2, 3, 4]],
                                       'output_names': ['import/z']})
            compiled = compiler.compile_source(ONNXModelFile(model_path=model_file.name), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='import/add/placeholder_port_0', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='import/add/placeholder_port_1', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='import/z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])
