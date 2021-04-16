# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import NamedTemporaryFile
from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.tf_frozen_graph_model_file_to_tf_model as compiler
from model_compiler.compilers.tf_frozen_graph_model_file_to_tf_model import Config
from model_compiler.models.irs.tf_frozen_graph_model import DataFormat
from model_compiler.models.sources.tf_frozen_graph_file import FrozenGraphFile


def _save_frozen_graph_model(model_file):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
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
            config = Config.from_json({'input_names': ['x:0', 'y:0'],
                                       'output_names': ['z:0'],
                                       'input_formats': ['channels_first', 'channels_last']})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertIsInstance(compiled.session, tf.compat.v1.Session)

        self.assertEqual([model_input.tensor.name for model_input in compiled.inputs], ['x:0', 'y:0'])
        self.assertEqual([model_input.data_format for model_input in compiled.inputs],
                         [DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST])

        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z:0'])

    def test_compile_with_no_input_name(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)

            config = Config.from_json({'output_names': ['z:0'],
                                       'input_formats': ['channels_first', 'channels_last']})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_input.tensor.name for model_input in compiled.inputs], ['x:0', 'y:0'])

    def test_compile_with_no_output_name(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)

            config = Config.from_json({'input_names': ['x:0', 'y:0'],
                                       'input_formats': ['channels_first', 'channels_last']})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z:0'])

    def test_compile_with_no_input_formats(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)

            config = Config.from_json({'input_names': ['x:0'],
                                       'output_names': ['z:0']})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_input.data_format for model_input in compiled.inputs], [None])

    def test_compile_with_input_formats_is_none(self):
        with NamedTemporaryFile(suffix='.pb') as model_file:
            _save_frozen_graph_model(model_file)

            config = Config.from_json({'input_names': ['x:0', 'y:0'],
                                       'output_names': ['z:0'],
                                       'input_formats': [None]})
            compiled = compiler.compile_source(FrozenGraphFile(model_path=model_file.name), config)

        self.assertEqual([model_input.data_format for model_input in compiled.inputs], [None, None])
