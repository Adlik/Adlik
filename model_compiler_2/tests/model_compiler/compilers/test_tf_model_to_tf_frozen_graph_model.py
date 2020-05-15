# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf
import tensorflow.python.eager.context as eager_context  # pylint: disable=no-name-in-module

import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as compiler
from model_compiler.models.irs import tf_model
from model_compiler.models.irs.tf_frozen_graph_model import Input


def _make_tensorflow_model(func):
    with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
        output_z = func(input_x, input_y, session)

    return tf_model.TensorFlowModel(inputs=[tf_model.Input(tensor=input_x), tf_model.Input(tensor=input_y)],
                                    outputs=[output_z],
                                    session=session)


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        tensorflow_model = _make_tensorflow_model(lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'))
        compiled = compiler.compile_source(source=tensorflow_model)

        self.assertEqual(compiled.inputs,
                         [Input(name='x:0', data_format=None), Input(name='y:0', data_format=None)])

        self.assertEqual(compiled.outputs, ['z:0'])

    def test_compile_with_variables(self):
        def _make_model(input_x, input_y, session):
            weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
            output_z = tf.multiply(input_x + input_y, weight, name='z')

            session.run(weight.initializer)

            return output_z

        frozen_graph_model = _make_tensorflow_model(_make_model)
        compiled = compiler.compile_source(source=frozen_graph_model)

        self.assertEqual(compiled.inputs,
                         [Input(name='x:0', data_format=None), Input(name='y:0', data_format=None)])

        self.assertEqual(compiled.outputs, ['z:0'])
