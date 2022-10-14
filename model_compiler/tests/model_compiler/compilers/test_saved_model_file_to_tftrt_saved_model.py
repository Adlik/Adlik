# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
from unittest import TestCase

import numpy as np
import pytest
import tensorflow as tf
from tensorflow.core.framework.types_pb2 import DataType as TfDataType  # pylint: disable=no-name-in-module

import model_compiler.compilers.saved_model_file_to_tftrt_saved_model as compiler
from model_compiler.compilers.saved_model_file_to_tftrt_saved_model import Config
from model_compiler.models.sources.saved_model_file import SavedModelFile
from model_compiler.models.targets.tftrt_saved_model import DataFormat
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _save_saved_model_file(model_dir):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='y')
        weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
        output_z = tf.add(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir)
        input_x_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_x)
        input_y_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_y)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output_z)

        builder.add_meta_graph_and_variables(
            sess=session,
            tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
            signature_def_map={
                'serving_default': tf.compat.v1.saved_model.build_signature_def(inputs={'x': input_x_tensor_info,
                                                                                        'y': input_y_tensor_info},
                                                                                outputs={'z': output_tensor_info})
            },
            clear_devices=True
        )

        builder.save()


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'signature_keys': 'predict', 'input_formats': ['channels_first'],
                                           'max_batch_size': 4}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=False, optimize_offline=False))

        self.assertEqual(Config.from_json({'signature_keys': 'predict', 'input_formats': ['channels_first'],
                                           'max_batch_size': 4, 'optimize_offline': True}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST],
                                max_batch_size=4, enable_fp16=False, optimize_offline=True))

        self.assertEqual(Config.from_json({'signature_keys': 'predict', 'input_formats': ['channels_first'],
                                           'max_batch_size': 4, 'enable_fp16': False, 'optimize_offline': False}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST],
                                max_batch_size=4, enable_fp16=False, optimize_offline=False))

        self.assertEqual(Config.from_json({'signature_keys': 'predict', 'input_formats': ['channels_first'],
                                           'max_batch_size': 4, 'enable_fp16': True, 'optimize_offline': False}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST],
                                max_batch_size=4, enable_fp16=True, optimize_offline=False))

        self.assertEqual(Config.from_json({'signature_keys': 'predict', 'input_formats': ['channels_first'],
                                           'max_batch_size': 4, 'enable_fp16': True, 'optimize_offline': True}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST],
                                max_batch_size=4, enable_fp16=True, optimize_offline=True))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'SIGNATURE_KEYS': 'predict', 'INPUT_FORMATS': 'channels_first',
                                          'MAX_BATCH_SIZE': '4'}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=False, optimize_offline=False))

        self.assertEqual(Config.from_env({'SIGNATURE_KEYS': 'predict', 'INPUT_FORMATS': 'channels_first',
                                          'MAX_BATCH_SIZE': '4', 'OPTIMIZE_OFFLINE': '1'}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=False, optimize_offline=True))

        self.assertEqual(Config.from_env({'SIGNATURE_KEYS': 'predict', 'INPUT_FORMATS': 'channels_first',
                                          'MAX_BATCH_SIZE': '4', 'ENABLE_FP16': '0', 'OPTIMIZE_OFFLINE': '0'}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=False, optimize_offline=False))

        self.assertEqual(Config.from_env({'SIGNATURE_KEYS': 'predict', 'INPUT_FORMATS': 'channels_first',
                                          'MAX_BATCH_SIZE': '4', 'ENABLE_FP16': '1', 'OPTIMIZE_OFFLINE': '0'}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=True, optimize_offline=False))

        self.assertEqual(Config.from_env({'SIGNATURE_KEYS': 'predict', 'INPUT_FORMATS': 'channels_first',
                                          'MAX_BATCH_SIZE': '4', 'ENABLE_FP16': '1', 'OPTIMIZE_OFFLINE': '1'}),
                         Config(signature_keys='predict', input_formats=[DataFormat.CHANNELS_FIRST], max_batch_size=4,
                                enable_fp16=True, optimize_offline=True))


def _input_fn():
    input_shapes = [[(1, 4), (1, 4)],
                    [(1, 4), (1, 4)]]
    for shapes in input_shapes:
        yield [tf.constant(np.zeros(x).astype(np.float32)) for x in shapes]


@pytest.mark.gpu_test
class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'])
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_with_input_formats_is_none(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config(max_batch_size=4, input_formats=None)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NONE, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NONE, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_with_less_input_formats(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config(max_batch_size=4, input_formats=['channels_last'])

            with self.assertRaises(ValueError) as context_manager:
                compiler.compile_source(SavedModelFile(model_path=model_dir), config)

        self.assertEqual(context_manager.exception.args,
                         ('Number of input formats (1) does not match number of inputs (2)',))

    def test_compile_with_signature_keys(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config(signature_keys='serving_default', max_batch_size=4,
                            input_formats=['channels_last', 'channels_last'])
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_with_optimize_offline(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            optimization_input_fn = _input_fn
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'], optimize_offline=True,
                            input_fn=optimization_input_fn)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_simple_fp16(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'], enable_fp16=True)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_fp16_with_optimize_offline(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            optimization_input_fn = _input_fn
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'], enable_fp16=True,
                            optimize_offline=True, input_fn=optimization_input_fn)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_simple_int8(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            calibration_input_fn = _input_fn
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'], enable_int8=True,
                            calibration_input_fn=calibration_input_fn)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])

    def test_compile_int8_with_optimize_offline(self):
        with TemporaryDirectory() as model_dir:
            _save_saved_model_file(model_dir)
            optimization_input_fn = _input_fn
            calibration_input_fn = _input_fn
            config = Config(max_batch_size=4, input_formats=['channels_last', 'channels_last'], enable_int8=True,
                            calibration_input_fn=calibration_input_fn, optimize_offline=True,
                            input_fn=optimization_input_fn)
            compiled = compiler.compile_source(SavedModelFile(model_path=model_dir), config)

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=TfDataType.DT_FLOAT,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z', data_type=TfDataType.DT_FLOAT, dims=[4])])
