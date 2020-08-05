# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import NamedTemporaryFile
from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.tf_frozen_graph_model_file_to_openvino_model as compiler
from model_compiler.compilers.tf_frozen_graph_model_file_to_openvino_model import Config, ModelInput
from model_compiler.models.data_format import DataFormat
from model_compiler.models.sources.tf_frozen_graph_file import FrozenGraphFile


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_names': ['input'],
                                           'input_formats': ['channels_first'],
                                           'output_names': ['output'],
                                           'max_batch_size': 1}),
                         Config(input_info=[('input', DataFormat.CHANNELS_FIRST)],
                                output_names=['output'],
                                max_batch_size=1))

    def test_from_json_no_names(self):
        self.assertEqual(Config.from_json({'input_names': None,
                                           'input_formats': None,
                                           'output_names': ['output'],
                                           'max_batch_size': 1}),
                         Config(input_info=None,
                                output_names=['output'],
                                max_batch_size=1))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_NAMES': 'input1,input2:0',
                                          'OUTPUT_NAMES': 'output',
                                          'INPUT_FORMATS': 'channels_first,channels_first',
                                          'MAX_BATCH_SIZE': '1'}),
                         Config(input_info=[('input1', DataFormat.CHANNELS_FIRST),
                                            ('input2:0', DataFormat.CHANNELS_FIRST)],
                                output_names=['output'],
                                max_batch_size=1))

    def test_from_env_no_names(self):
        self.assertEqual(Config.from_env({'OUTPUT_NAMES': 'output',
                                          'MAX_BATCH_SIZE': '1'}),
                         Config(input_info=None,
                                output_names=['output'],
                                max_batch_size=1))


def _save_frozen_graph_model(model_file):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        tf.multiply(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        constant_graph = tf.compat.v1.graph_util.convert_variables_to_constants(session, session.graph_def, ['z'])

    with open(model_file.name, mode='wb') as graph_file:
        graph_file.write(constant_graph.SerializeToString())


class CompileSourceTestCase(TestCase):
    def test_compile_with_variables(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'input_formats': ['channels_first', 'channels_first'],
                                       'max_batch_size': 1})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_input.name for model_input in compiled.inputs], ['x', 'y'])
        self.assertEqual([model_input.format for model_input in compiled.inputs],
                         [ModelInput.FORMAT_NCHW, ModelInput.FORMAT_NCHW])  # pylint: disable=no-member
        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z'])

    def test_compile_with_variables_channel_last(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'input_formats': ['channels_last', 'channels_last'],
                                       'max_batch_size': 1})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_input.name for model_input in compiled.inputs], ['x', 'y'])
        self.assertEqual([model_input.format for model_input in compiled.inputs],
                         [ModelInput.FORMAT_NCHW, ModelInput.FORMAT_NCHW])  # pylint: disable=no-member
        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z'])

    def test_compile_with_variables_input_name_format_different_length(self):
        self.assertRaises(ValueError, Config.from_json,
                          {'input_names': ['x', 'y'],
                           'output_names': ['z'],
                           'input_formats': ['channels_last'],
                           'max_batch_size': 1})

    def test_compile_with_no_input_name(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)
            config = Config.from_json({'input_names': None,
                                       'output_names': ['z'],
                                       'max_batch_size': 1})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)
        self.assertEqual(compiled.inputs, None)

    def test_compile_with_no_input_formats(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'max_batch_size': 4})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)
        self.assertEqual([model_input.format for model_input in compiled.inputs],
                         [ModelInput.FORMAT_NONE, ModelInput.FORMAT_NONE])  # pylint: disable=no-member
        self.assertEqual([model_input.name for model_input in compiled.inputs], ['x', 'y'])

    def test_compile_with_input_formats_is_none(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'input_formats': [None, None],
                                       'max_batch_size': 4})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)
        self.assertEqual([model_input.format for model_input in compiled.inputs],
                         [ModelInput.FORMAT_NONE, ModelInput.FORMAT_NONE])  # pylint: disable=no-member
        self.assertEqual([model_input.name for model_input in compiled.inputs], ['x', 'y'])
