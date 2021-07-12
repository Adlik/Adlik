# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase
import tensorflow as tf
import tvm
import tvm.relay as relay

from model_compiler.models.targets.tvm_model import DataFormat, TvmModel, Input, Output
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_simple_tvm_model() -> TvmModel:
    origin_model = tf.keras.Sequential()
    origin_model.add(tf.keras.layers.Dense(units=8, name='l1', input_shape=(16, )))
    origin_model.add(tf.keras.layers.Dense(units=4, name='l2'))
    origin_model.add(tf.keras.layers.Dense(units=2, name='l3'))
    model, params = relay.frontend.from_keras(origin_model, {'l1_input': [1, 16]})
    compiled_lib = relay.build(model, tvm.target.create("llvm"), params=params)

    return TvmModel(tvm_model=compiled_lib,
                    model_inputs=[Input(name='l1_input', shape=[1, 16], data_type='DT_INT8',
                                        data_format=DataFormat.CHANNELS_FIRST)],
                    model_outputs=[Output(name='Identity', shape=[1, 2], data_type='DT_FLOAT')])


class TvmModelFileTestCase(TestCase):
    def test_get_single_inputs(self):
        tvm_model = _make_simple_tvm_model()

        self.assertEqual(tvm_model.get_inputs(), [ModelInput(name='l1_input', data_type='DT_INT8',
                                                             format='FORMAT_NCHW', dims=[16])])

    def test_get_outputs(self):
        tvm_model = _make_simple_tvm_model()

        self.assertEqual(tvm_model.get_outputs(), [ModelOutput(name='Identity', data_type='DT_FLOAT', dims=[2])])

    def test_save(self):
        tvm_model = _make_simple_tvm_model()

        with TemporaryDirectory() as save_path:
            tvm_model.save(save_path)

            self.assertEqual(os.listdir(save_path), ['model.so'])

    def test_get_platform(self):
        self.assertEqual(TvmModel.get_platform(), ('tvm', '0.7.0'))
