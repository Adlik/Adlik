# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase
import tensorflow as tf
from model_compiler.models.targets.tflite_model import TfLiteModel, DataFormat
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_tflite_model() -> TfLiteModel:
    input_x = [-1, 0, 1, 2, 3, 4]
    output_y = [-3, -1, 1, 3, 5, 7]

    model = tf.keras.models.Sequential([tf.keras.layers.Dense(units=1, input_shape=[1])])
    model.compile(optimizer='sgd', loss='mean_squared_error')
    model.fit(input_x, output_y, epochs=1)
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    return TfLiteModel(tflite_model=tflite_model, input_formats=[DataFormat.CHANNELS_LAST])


class TfLiteModelFileTestCase(TestCase):
    def test_get_inputs(self):
        tflite_model = _make_tflite_model()

        self.assertEqual(tflite_model.get_inputs(), [ModelInput(name='dense_input', data_type='DT_FLOAT',
                                                                format='FORMAT_NHWC', dims=[1])])

    def test_get_outputs(self):
        tflite_model = _make_tflite_model()

        self.assertEqual(tflite_model.get_outputs(), [ModelOutput(name='Identity', data_type='DT_FLOAT', dims=[1])])

    def test_save(self):
        tflite_model = _make_tflite_model()

        with TemporaryDirectory() as save_path:
            tflite_model.save(save_path)

            self.assertEqual(os.listdir(save_path), ['model.tflite'])

    def test_get_platform(self):
        self.assertEqual(TfLiteModel.get_platform(), ('tflite', tf.__version__))
