# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.saved_model_file_to_openvino_model as compiler
from model_compiler.compilers.saved_model_file_to_openvino_model import Config
from model_compiler.models.sources.saved_model_file import SavedModelFile
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


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
        weight_tensor_info = tf.compat.v1.saved_model.build_tensor_info(weight)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output)

        builder.add_meta_graph_and_variables(session, tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
                                             signature_def_map={
                                                 'predict': tf.compat.v1.saved_model.build_signature_def(
                                                     inputs={'x': input_x_tensor_info,
                                                             'y': input_y_tensor_info,
                                                             'weight': weight_tensor_info},
                                                     outputs={'z': output_tensor_info})})
        builder.save()


def _save_saved_model_file_16(model_dir):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float16, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float16, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float16)
        output = tf.multiply(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir)
        input_x_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_x)
        input_y_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_y)
        weight_tensor_info = tf.compat.v1.saved_model.build_tensor_info(weight)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output)

        builder.add_meta_graph_and_variables(session, tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
                                             signature_def_map={
                                                 'predict': tf.compat.v1.saved_model.build_signature_def(
                                                     inputs={'x': input_x_tensor_info,
                                                             'y': input_y_tensor_info,
                                                             'weight': weight_tensor_info},
                                                     outputs={'z': output_tensor_info})})
        builder.save()


def _save_saved_model_file_with_two_tags(model_dir):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight1 = tf.Variable(initial_value=4.2, dtype=tf.float32)
        output = tf.multiply(input_x + input_y, weight1, name='z')

        session.run(weight1.initializer)

        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir)
        input_x_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_x)
        input_y_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_y)
        weight_tensor_info = tf.compat.v1.saved_model.build_tensor_info(weight1)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output)

        builder.add_meta_graph_and_variables(session, tags=[tf.compat.v1.saved_model.tag_constants.SERVING, 'graph2'],
                                             signature_def_map={
                                                 'predict': tf.compat.v1.saved_model.build_signature_def(
                                                     inputs={'x': input_x_tensor_info,
                                                             'y': input_y_tensor_info,
                                                             'weight': weight_tensor_info},
                                                     outputs={'z': output_tensor_info})})
        builder.save()

        weight2 = tf.Variable(initial_value=2.5, dtype=tf.float32)
        session.run(weight2.initializer)
        # builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir2 + '2')
        # builder.add_meta_graph_and_variables(session, tags=['graph2'])
        builder.add_meta_graph(tags=['graph2'])
        builder.save()


class CompileSourceTestCase(TestCase):
    def test_compile_with_no_params(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config.from_json({'max_batch_size': 1})
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y:0', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='z:0', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_fp16(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file_16(model_dir)
            config = Config.from_json({'max_batch_size': 1,
                                       'data_type': 'FP16'})
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=tf.float16.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y:0', data_type=tf.float16.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='z:0', data_type=tf.float16.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_all_params_with_shape(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'input_shapes': [[1, 2, 3, 4], [1, 2, 3, 4]],
                                       'output_names': ['z']
                                       })
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_saved_model_tags(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'max_batch_size': 1,
                                       'saved_model_tags': ['serve']})
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_two_saved_model_tags(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file_with_two_tags(model_dir)
            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'max_batch_size': 1,
                                       'saved_model_tags': ['serve', 'graph2']})
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)
            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
            self.assertEqual(compiled.get_outputs(),
                             [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])
