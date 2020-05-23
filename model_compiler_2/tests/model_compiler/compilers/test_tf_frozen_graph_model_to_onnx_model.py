# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf
import tensorflow.python.eager.context as eager_context

import model_compiler.compilers.tf_frozen_graph_model_to_onnx_model as compiler
import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as frozen_graph_compiler
from model_compiler.models.irs.tf_model import Input, TensorFlowModel


def _make_frozen_graph_model(func):
    with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
        output_z = func(input_x, input_y, session)

    return frozen_graph_compiler.compile_source(
        source=TensorFlowModel(inputs=[Input(tensor=input_x), Input(tensor=input_y)],
                               outputs=[output_z],
                               session=session)
    )


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        frozen_graph_model = _make_frozen_graph_model(lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'))
        compiled = compiler.compile_source(source=frozen_graph_model)
        compiled_graph = compiled.model_proto.graph

        self.assertEqual([graph_input.name for graph_input in compiled_graph.input], ['x:0', 'y:0'])
        self.assertEqual(compiled.input_data_formats, [None, None])

    def test_compile_with_variables(self):
        def _make_model(input_x, input_y, session):
            weight = tf.Variable(initial_value=4.2, dtype=tf.float32, name='w')
            output_z = tf.multiply(input_x + input_y, weight, name='z')

            session.run(weight.initializer)

            return output_z

        frozen_graph_model = _make_frozen_graph_model(_make_model)
        compiled = compiler.compile_source(source=frozen_graph_model)
        compiled_graph = compiled.model_proto.graph

        self.assertEqual([graph_input.name for graph_input in compiled_graph.input], ['x:0', 'y:0'])
        self.assertEqual(compiled.input_data_formats, [None, None])
