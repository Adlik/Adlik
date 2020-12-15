# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import contextlib
from unittest import TestCase

import pytest
import tensorflow as tf
from tensorflow.core.framework.types_pb2 import DataType as TfDataType  # pylint: disable=no-name-in-module
from tensorrt import ICudaEngine, IInt8EntropyCalibrator2

import model_compiler.compilers.onnx_model_to_tensorrt_model as compiler
import model_compiler.compilers.tf_frozen_graph_model_to_onnx_model as onnx_compiler
import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as frozen_graph_compiler
from model_compiler.compilers.onnx_model_to_tensorrt_model import Config
from model_compiler.models.irs.tf_model import Input as TfInput, TensorFlowModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=False),
                         Config.from_json({'max_batch_size': 7}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=False),
                         Config.from_json({'max_batch_size': 7, 'enable_fp16': False}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=True, enable_strict_types=False),
                         Config.from_json({'max_batch_size': 7, 'enable_fp16': True}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=True),
                         Config.from_json({'max_batch_size': 7, 'enable_strict_types': True}))

    def test_from_env(self):
        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=False),
                         Config.from_env({'MAX_BATCH_SIZE': '7'}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=False),
                         Config.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_FP16': '0'}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=True, enable_strict_types=False),
                         Config.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_FP16': '1'}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=False),
                         Config.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_STRICT_TYPES': '0'}))

        self.assertEqual(Config(max_batch_size=7, enable_fp16=False, enable_strict_types=True),
                         Config.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_STRICT_TYPES': '1'}))


def _make_onnx_model(func, batch_size_1, batch_size_2):
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[batch_size_1, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[batch_size_2, 4], name='y')
        output_z = func(input_x, input_y, session)

    frozen_graph_model = frozen_graph_compiler.compile_source(
        source=TensorFlowModel(inputs=[TfInput(tensor=input_x), TfInput(tensor=input_y)],
                               outputs=[output_z],
                               session=session)
    )

    return onnx_compiler.compile_source(frozen_graph_model)


@pytest.mark.gpu_test
class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        for batch_size in [3, None]:
            onnx_model = _make_onnx_model(func=lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'),
                                          batch_size_1=batch_size,
                                          batch_size_2=batch_size)

            compiled = compiler.compile_source(source=onnx_model, config=Config(max_batch_size=4))

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4]),
                              ModelInput(name='y:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4])])

            self.assertEqual(compiled.input_data_formats, [None, None])
            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z:0', data_type=TfDataType.DT_FLOAT, dims=[4])])
            self.assertIsInstance(compiled.cuda_engine, ICudaEngine)

    def test_compile_inconsistent_batch_size(self):
        onnx_model = _make_onnx_model(func=lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'),
                                      batch_size_1=3,
                                      batch_size_2=None)

        with self.assertRaises(ValueError) as error:
            compiler.compile_source(source=onnx_model, config=Config(max_batch_size=4))

        self.assertEqual(error.exception.args, ('Inconsistent batch size specification.',))

    def test_compile_fp16(self):
        def _build_model(input_x, input_y, session):
            weight = tf.Variable(initial_value=0.0, dtype=tf.float32, name='w')

            session.run(weight.initializer)

            return tf.multiply(weight, input_x + input_y, name='z')

        for batch_size in [3, None]:
            onnx_model = _make_onnx_model(func=_build_model, batch_size_1=batch_size, batch_size_2=batch_size)

            compiled = compiler.compile_source(source=onnx_model,
                                               config=Config(max_batch_size=4,
                                                             enable_fp16=True,
                                                             enable_strict_types=True))

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4]),
                              ModelInput(name='y:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4])])

            self.assertEqual(compiled.input_data_formats, [None, None])
            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z:0', data_type=TfDataType.DT_FLOAT, dims=[4])])
            self.assertIsInstance(compiled.cuda_engine, ICudaEngine)

    def test_compile_int8(self):
        from .. import mini_cuda  # pylint: disable=import-outside-toplevel

        def _build_model(input_x, input_y, session):
            weight = tf.Variable(initial_value=0.0, dtype=tf.float32, name='w')

            session.run(weight.initializer)

            return tf.multiply(weight, input_x + input_y, name='z')

        class _MyCalibrator(IInt8EntropyCalibrator2):
            def __init__(self):
                super().__init__()

                self._buffers = [mini_cuda.allocate_memory(4 * 4 * 3), mini_cuda.allocate_memory(4 * 4 * 3)]
                self._cache = None
                self._index = 0

            def close(self):
                for buffer in self._buffers:
                    buffer.close()

            def get_batch(self, names, p_str=None):
                del names, p_str

                if self._index == 16:
                    return None

                self._index += 1

                return list(map(int, self._buffers))

            def get_batch_size(self):
                return 3

            def read_calibration_cache(self):
                return self._cache

            def write_calibration_cache(self, cache):
                self._cache = cache

        mini_cuda.init()

        for batch_size in [3, None]:
            onnx_model = _make_onnx_model(func=_build_model, batch_size_1=batch_size, batch_size_2=batch_size)

            with contextlib.closing(mini_cuda.get_device(0).create_context(0)), \
                 contextlib.closing(_MyCalibrator()) as calibrator:
                compiled = compiler.compile_source(source=onnx_model,
                                                   config=Config(max_batch_size=4,
                                                                 int8_calibrator=calibrator,
                                                                 enable_int8=True,
                                                                 enable_fp16=True,
                                                                 enable_strict_types=True))

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4]),
                              ModelInput(name='y:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4])])

            self.assertEqual(compiled.input_data_formats, [None, None])
            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z:0', data_type=TfDataType.DT_FLOAT, dims=[4])])
            self.assertIsInstance(compiled.cuda_engine, ICudaEngine)
