# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase
from tempfile import TemporaryDirectory

import tensorflow as tf

import model_compiler.compilers.saved_model_file_to_tflite_model as compiler
from model_compiler.compilers.saved_model_file_to_tflite_model import Config
from model_compiler.models.sources.saved_model_file import SavedModelFile
from model_compiler.models.targets.tflite_model import DataFormat


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


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config(input_formats=[]), Config.from_json({}))
        self.assertEqual(Config(input_formats=[DataFormat.CHANNELS_FIRST]),
                         Config.from_json({'input_formats': ['channels_first']}))
        self.assertEqual(Config(input_formats=[DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST]),
                         Config.from_json({'input_formats': ['channels_first', 'channels_last']}))

        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=None),
                         Config.from_json({}))
        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=None),
                         Config.from_json({'optimization': False}))
        self.assertEqual(Config(input_formats=[], optimization=True, supported_types=None),
                         Config.from_json({'optimization': True}))

        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=[tf.float16]),
                         Config.from_json({'supported_types': ['float16']}))

        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=[tf.float16, tf.float32]),
                         Config.from_json({'supported_types': ['float16', 'float32']}))

        self.assertEqual(Config(input_formats=[], optimization=True, supported_types=[tf.float16, tf.float32]),
                         Config.from_json({'optimization': True, 'supported_types': ['float16', 'float32']}))

        self.assertEqual(Config(input_formats=[], supported_ops=None), Config.from_json({}))

        self.assertEqual(Config(input_formats=[], supported_ops=[tf.lite.OpsSet.TFLITE_BUILTINS_INT8]),
                         Config.from_json({'supported_ops': ['TFLITE_BUILTINS_INT8']}))

        self.assertEqual(Config(input_formats=[],
                                supported_ops=[tf.lite.OpsSet.SELECT_TF_OPS, tf.lite.OpsSet.TFLITE_BUILTINS_INT8]),
                         Config.from_json({'supported_ops': ['SELECT_TF_OPS', 'TFLITE_BUILTINS_INT8']}))

        self.assertEqual(Config(input_formats=[], inference_input_type=tf.float32),
                         Config.from_json({}))
        self.assertEqual(Config(input_formats=[], inference_input_type=tf.float32),
                         Config.from_json({'inference_input_type': 'float32'}))

        self.assertEqual(Config(input_formats=[], inference_output_type=tf.float32),
                         Config.from_json({}))
        self.assertEqual(Config(input_formats=[], inference_output_type=tf.float32),
                         Config.from_json({'inference_output_type': 'float32'}))

    def test_from_env(self):
        self.assertEqual(Config(input_formats=[]), Config.from_env({}))
        self.assertEqual(Config(input_formats=[DataFormat.CHANNELS_FIRST]),
                         Config.from_env({'INPUT_FORMATS': 'channels_first'}))
        self.assertEqual(Config(input_formats=[DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST]),
                         Config.from_env({'INPUT_FORMATS': 'channels_first,channels_last'}))

        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=None),
                         Config.from_env({}))
        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=None),
                         Config.from_env({'OPTIMIZATION': '0'}))
        self.assertEqual(Config(input_formats=[], optimization=True, supported_types=None),
                         Config.from_env({'OPTIMIZATION': '1'}))

        self.assertEqual(Config(input_formats=[], optimization=False, supported_types=[tf.float16]),
                         Config.from_env({'SUPPORTED_TYPES': 'float16'}))

        self.assertEqual(Config(input_formats=[],
                                optimization=False, supported_types=[tf.float16, tf.float32]),
                         Config.from_env({'SUPPORTED_TYPES': 'float16,float32'}))

        self.assertEqual(Config(input_formats=[],
                                optimization=True, supported_types=[tf.float16, tf.float32]),
                         Config.from_env({'OPTIMIZATION': '1', 'SUPPORTED_TYPES': 'float16,float32'}))

        self.assertEqual(Config(input_formats=[], supported_ops=None), Config.from_env({}))

        self.assertEqual(Config(input_formats=[], supported_ops=[tf.lite.OpsSet.TFLITE_BUILTINS_INT8]),
                         Config.from_env({'SUPPORTED_OPS': 'TFLITE_BUILTINS_INT8'}))

        self.assertEqual(Config(input_formats=[],
                                supported_ops=[tf.lite.OpsSet.SELECT_TF_OPS, tf.lite.OpsSet.TFLITE_BUILTINS_INT8]),
                         Config.from_env({'SUPPORTED_OPS': 'SELECT_TF_OPS,TFLITE_BUILTINS_INT8'}))

        self.assertEqual(Config(input_formats=[], inference_input_type=tf.float32), Config.from_env({}))
        self.assertEqual(Config(input_formats=[], inference_input_type=tf.float32),
                         Config.from_env({'INFERENCE_INPUT_TYPE': 'float32'}))

        self.assertEqual(Config(input_formats=[],
                                inference_output_type=tf.float32), Config.from_env({}))
        self.assertEqual(Config(input_formats=[], inference_output_type=tf.float32),
                         Config.from_env({'INFERENCE_OUTPUT_TYPE': 'float32'}))

    def test_invalid_data_type(self):
        with self.assertRaises(ValueError):
            Config.from_json({'supported_types': ['foobar']})

        with self.assertRaises(ValueError):
            Config.from_json({'supported_types': ['as_dtype']})


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            compiled = compiler.compile_source(source=SavedModelFile(model_path=model_dir),
                                               config=Config(input_formats=['channels_first', 'channels_last']))

            self.assertIsInstance(compiled.tflite_model, bytes)

    def test_compile_simple_fp16(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            compiled_1 = compiler.compile_source(source=SavedModelFile(model_path=model_dir),
                                                 config=Config(input_formats=[]))
            compiled_2 = compiler.compile_source(source=SavedModelFile(model_path=model_dir),
                                                 config=Config(optimization=True, supported_types=[tf.float16],
                                                               input_formats=['']))

            self.assertLess(len(compiled_1.tflite_model), len(compiled_2.tflite_model))

    def test_compile_select_tf_op(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            compiled = compiler.compile_source(source=SavedModelFile(model_path=model_dir),
                                               config=Config(supported_ops=[tf.lite.OpsSet.SELECT_TF_OPS],
                                                             input_formats=['channels_first']))

            self.assertIsInstance(compiled.tflite_model, bytes)
