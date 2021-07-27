# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from tempfile import NamedTemporaryFile
from tensorflow import keras

import model_compiler.compilers.keras_model_file_to_tvm_model as compiler
from model_compiler.keras_util import Config, DataFormat
from model_compiler.models.sources.keras_model_file import KerasModelFile


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with NamedTemporaryFile(suffix='.h5') as model_file:
            origin_model = keras.Sequential()
            origin_model.add(keras.layers.Dense(units=8, name='l1', input_shape=(16,)))
            origin_model.add(keras.layers.Dense(units=4, name='l2'))
            origin_model.add(keras.layers.Dense(units=2, name='l3'))
            origin_model.save(model_file.name)
            compiled = compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                               config=Config(input_nodes=None, output_nodes=None, max_batch_size=1,
                                                             target='llvm', need_search_schedule=False))

        self.assertEqual(len(compiled.model_inputs), 1)
        self.assertEqual(compiled.model_inputs[0].name, origin_model.inputs[0].name)
        self.assertEqual(compiled.model_inputs[0].data_format, DataFormat.CHANNELS_FIRST)

        self.assertEqual(len(compiled.model_outputs), 1)
        self.assertEqual(compiled.model_outputs[0].name, origin_model.outputs[0].name)

    def test_compile_channels_last_model(self):
        with NamedTemporaryFile(suffix='.h5') as model_file:
            model = keras.models.Sequential()
            model.add(keras.layers.Conv2D(32, kernel_size=(3, 3),
                                          activation='relu',
                                          input_shape=(28, 28, 1)))
            model.save(model_file.name)
            compiled = compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                               config=Config(input_nodes=None, output_nodes=None, max_batch_size=1,
                                                             target='llvm', need_search_schedule=False))

        self.assertEqual(len(compiled.model_inputs), 1)
        self.assertEqual(compiled.model_inputs[0].name, model.inputs[0].name)
        self.assertEqual(compiled.model_inputs[0].data_format, DataFormat.CHANNELS_FIRST)

        self.assertEqual(len(compiled.model_outputs), 1)
        self.assertEqual(compiled.model_outputs[0].name, model.outputs[0].name)

    def test_compile_with_schedule_search(self):
        with NamedTemporaryFile(suffix='.h5') as model_file:
            model = keras.models.Sequential()
            model.add(keras.layers.Conv2D(8, kernel_size=(3, 3),
                                          activation='relu',
                                          input_shape=(3, 28, 28),
                                          name='input'))
            model.add(keras.layers.Conv2D(8, kernel_size=(1, 1),
                                          activation='relu'))
            model.save(model_file.name)

            searched_compiled = compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                                        config=Config(input_nodes=None, output_nodes=None,
                                                                      max_batch_size=1, need_search_schedule=True,
                                                                      target='llvm'))

            self.assertEqual(len(searched_compiled.model_inputs), 1)
            self.assertEqual(searched_compiled.model_inputs[0].name, model.inputs[0].name)
            self.assertEqual(searched_compiled.model_inputs[0].data_format, DataFormat.CHANNELS_FIRST)

            self.assertEqual(len(searched_compiled.model_outputs), 1)
            self.assertEqual(searched_compiled.model_outputs[0].name, model.outputs[0].name)

    def test_compile_target_not_supported_or_illegal(self):
        with NamedTemporaryFile(suffix='.h5') as model_file:
            model = keras.models.Sequential()
            model.add(keras.layers.Conv2D(8, kernel_size=(3, 3),
                                          activation='relu',
                                          input_shape=(3, 28, 28)))
            model.add(keras.layers.Conv2D(8, kernel_size=(1, 1),
                                          activation='relu'))
            model.save(model_file.name)

            try:
                compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                        config=Config(target='arm-cpu'))
            except NotImplementedError as exception:
                self.assertEqual(exception.args[0], 'Arm compile not supported yet')

            try:
                compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                        config=Config(target='cuda'))
            except NotImplementedError as exception:
                self.assertEqual(exception.args[0], 'CUDA compile not supported yet')

            try:
                compiler.compile_source(source=KerasModelFile(model_path=model_file.name),
                                        config=Config(target='npu'))
            except NameError as exception:
                self.assertEqual(exception.args[0], 'Not supported target name: "npu"')
