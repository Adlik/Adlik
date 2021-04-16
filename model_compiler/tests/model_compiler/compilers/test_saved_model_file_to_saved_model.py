# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase
from tempfile import TemporaryDirectory

import tensorflow as tf

import model_compiler.compilers.saved_model_file_to_saved_model as compiler
from model_compiler.tensorflow_util import Config
from model_compiler.models.sources.saved_model_file import SavedModelFile
from model_compiler.models.data_format import DataFormat


def _save_saved_model_file(model_dir):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        output = tf.multiply(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir)
        input_x_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_x)
        input_y_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_y)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output)

        builder.add_meta_graph_and_variables(session, tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
                                             signature_def_map={
                                                 'predict': tf.compat.v1.saved_model.build_signature_def(
                                                     inputs={'x': input_x_tensor_info,
                                                             'y': input_y_tensor_info},
                                                     outputs={'z': output_tensor_info})})
        builder.save()


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            compiled = compiler.compile_source(source=SavedModelFile(model_path=model_dir),
                                               config=Config.from_json({"input_formats":
                                                                        ['channels_first', 'channels_last']}))

        self.assertEqual([model_input.tensor.name for model_input in compiled.inputs], ['x:0', 'y:0'])
        self.assertEqual([model_input.data_format for model_input in compiled.inputs],
                         [DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST])
        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z:0'])
