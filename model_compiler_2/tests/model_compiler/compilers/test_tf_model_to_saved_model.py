# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf
import tensorflow.python.eager.context as eager_context

import model_compiler.compilers.tf_model_to_saved_model as compiler
from model_compiler.compilers.tf_model_to_saved_model import Config
from model_compiler.models.irs.tf_model import Input, TensorFlowModel


class ConfigTestCase(TestCase):
    def test_from_json_minimal(self):
        config = Config.from_json({})

        self.assertEqual(config, Config(input_signature=None, output_signature=None))

    def test_from_json_input_names_only(self):
        config = Config.from_json({'input_signatures': ['abc', 'def']})

        self.assertEqual(config, Config(input_signature=['abc', 'def'], output_signature=None))

    def test_from_json_output_names_only(self):
        config = Config.from_json({'output_signatures': ['abc', 'def']})

        self.assertEqual(config, Config(input_signature=None, output_signature=['abc', 'def']))

    def test_from_json_full(self):
        config = Config.from_json({'input_signatures': ['abc', 'def'],
                                   'output_signatures': ['ghi', 'jkl', 'mno']})

        self.assertEqual(config, Config(input_signature=['abc', 'def'], output_signature=['ghi', 'jkl', 'mno']))


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
            output_z = tf.add(input_x, input_y, name='z')

        compiled = compiler.compile_source(source=TensorFlowModel(inputs=[Input(tensor=input_x), Input(tensor=input_y)],
                                                                  outputs=[output_z],
                                                                  session=session),
                                           config=Config())

        self.assertEqual(len(compiled.inputs), 2)
        self.assertEqual(compiled.inputs[0].name, 'x:0')
        self.assertEqual(compiled.inputs[0].tensor, input_x)
        self.assertIsNone(compiled.inputs[0].data_format)
        self.assertEqual(compiled.inputs[1].name, 'y:0')
        self.assertEqual(compiled.inputs[1].tensor, input_y)
        self.assertIsNone(compiled.inputs[1].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertEqual(compiled.outputs[0].name, 'z:0')
        self.assertIs(compiled.outputs[0].tensor, output_z)

        self.assertIs(compiled.session, session)

    def test_compile_with_explicit_signature(self):
        with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
            input_z = tf.add(input_x, input_y, name='z')

        compiled = compiler.compile_source(source=TensorFlowModel(inputs=[Input(tensor=input_x), Input(tensor=input_y)],
                                                                  outputs=[input_z],
                                                                  session=session),
                                           config=Config(input_signature=['foo', 'bar'], output_signature=['baz']))

        self.assertEqual(len(compiled.inputs), 2)
        self.assertEqual(compiled.inputs[0].name, 'foo')
        self.assertEqual(compiled.inputs[0].tensor, input_x)
        self.assertIsNone(compiled.inputs[0].data_format)
        self.assertEqual(compiled.inputs[1].name, 'bar')
        self.assertEqual(compiled.inputs[1].tensor, input_y)
        self.assertIsNone(compiled.inputs[1].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertEqual(compiled.outputs[0].name, 'baz')
        self.assertIs(compiled.outputs[0].tensor, input_z)

        self.assertIs(compiled.session, session)
