# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf
from tensorflow import keras

import model_compiler.compilers.keras_model_to_tf_model as compiler
from model_compiler.compilers.keras_model_to_tf_model import Config, KerasModel, NodeSpec
from model_compiler.models.irs.tf_model import DataFormat


class NodeSpecTestCase(TestCase):
    def test_name_only(self):
        self.assertEqual(NodeSpec.from_str('abc'), NodeSpec(layer_name='abc', node_index=0))

    def test_name_and_node_index(self):
        self.assertEqual(NodeSpec.from_str('abc:4'), NodeSpec(layer_name='abc', node_index=4))

    def test_invalid_name(self):
        with self.assertRaises(ValueError):
            NodeSpec.from_str('abc:4:3')


class ConfigTestCase(TestCase):
    def test_from_json_minimal(self):
        config = Config.from_json({})

        self.assertEqual(config, Config(input_nodes=None, output_nodes=None))

    def test_from_json_input_names_only(self):
        config = Config.from_json({'input_layer_names': ['abc', 'def:3']})

        self.assertEqual(config, Config(input_nodes=[NodeSpec(layer_name='abc'),
                                                     NodeSpec(layer_name='def', node_index=3)],
                                        output_nodes=None))

    def test_from_json_output_names_only(self):
        config = Config.from_json({'output_layer_names': ['abc', 'def:3']})

        self.assertEqual(config, Config(input_nodes=None,
                                        output_nodes=[NodeSpec(layer_name='abc'),
                                                      NodeSpec(layer_name='def', node_index=3)]))

    def test_from_json_full(self):
        config = Config.from_json({'input_layer_names': ['abc', 'def:3'],
                                   'output_layer_names': ['ghi:2', 'jkl']})

        self.assertEqual(config, Config(input_nodes=[NodeSpec(layer_name='abc'),
                                                     NodeSpec(layer_name='def', node_index=3)],
                                        output_nodes=[NodeSpec(layer_name='ghi', node_index=2),
                                                      NodeSpec(layer_name='jkl')]))

    def test_from_env_minimal(self):
        config = Config.from_env({})

        self.assertEqual(config, Config(input_nodes=None, output_nodes=None))

    def test_from_env_input_names_only(self):
        config = Config.from_env({'INPUT_LAYER_NAMES': 'abc,def:3'})

        self.assertEqual(config, Config(input_nodes=[NodeSpec(layer_name='abc'),
                                                     NodeSpec(layer_name='def', node_index=3)],
                                        output_nodes=None))

    def test_from_env_output_names_only(self):
        config = Config.from_env({'OUTPUT_LAYER_NAMES': 'abc,def:3'})

        self.assertEqual(config, Config(input_nodes=None,
                                        output_nodes=[NodeSpec(layer_name='abc'),
                                                      NodeSpec(layer_name='def', node_index=3)]))

    def test_from_env_full(self):
        config = Config.from_env({'INPUT_LAYER_NAMES': 'abc,def:3',
                                  'OUTPUT_LAYER_NAMES': 'ghi:2,jkl'})

        self.assertEqual(config, Config(input_nodes=[NodeSpec(layer_name='abc'),
                                                     NodeSpec(layer_name='def', node_index=3)],
                                        output_nodes=[NodeSpec(layer_name='ghi', node_index=2),
                                                      NodeSpec(layer_name='jkl')]))


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            model = KerasModel(model=keras.Sequential([keras.layers.Dense(units=4, input_shape=[8])]), session=session)

        compiled = compiler.compile_source(source=model, config=Config())

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertIs(compiled.outputs[0], model.model.output)

        self.assertIs(compiled.session, session)

    def test_compile_simple_with_input_layer(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            model = KerasModel(model=keras.Sequential([keras.layers.InputLayer(input_shape=[8]),
                                                       keras.layers.Dense(units=4)]),
                               session=session)

        compiled = compiler.compile_source(source=model, config=Config())

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertIs(compiled.outputs[0], model.model.output)

        self.assertIs(compiled.session, session)

    def test_compile_with_input_name(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            origin_model = keras.Sequential()
            origin_model.add(keras.layers.Dense(units=8, name='l1', input_shape=(16,)))
            origin_model.add(keras.layers.Dense(units=4, name='l2'))
            origin_model.add(keras.layers.Dense(units=2, name='l3'))

        model = KerasModel(model=origin_model, session=session)

        compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l2')]))

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, origin_model.layers[1].input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertIs(compiled.outputs[0], model.model.output)

        self.assertIs(compiled.session, session)

    def test_compile_with_input_name_to_input_layer(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_tensor = keras.layers.Input(shape=[16], name='l0')
            output_tensor = keras.layers.Dense(units=8)(input_tensor)

        model = KerasModel(model=keras.Model(inputs=[input_tensor], outputs=[output_tensor]), session=session)
        compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertIs(compiled.outputs[0], model.model.output)

        self.assertIs(compiled.session, session)

    def test_compile_with_output_name(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            origin_model = keras.Sequential()
            origin_model.add(keras.layers.Dense(units=8, name='l1', input_shape=(16,)))
            origin_model.add(keras.layers.Dense(units=4, name='l2'))
            origin_model.add(keras.layers.Dense(units=2, name='l3'))

        model = KerasModel(model=origin_model, session=session)
        compiled = compiler.compile_source(source=model, config=Config(output_nodes=[NodeSpec(layer_name='l2')]))

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 1)
        self.assertIs(compiled.outputs[0], origin_model.layers[1].output)

        self.assertIs(compiled.session, session)

    def test_compile_simple_with_data_format(self):
        for (keras_data_format, data_format) in [('channels_first', DataFormat.CHANNELS_FIRST),
                                                 ('channels_last', DataFormat.CHANNELS_LAST)]:
            with self.subTest(data_format=keras_data_format):
                with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
                    model = KerasModel(model=keras.Sequential([keras.layers.Conv2D(filters=4,
                                                                                   kernel_size=(3, 3),
                                                                                   data_format=keras_data_format,
                                                                                   input_shape=(28, 28, 3))]),
                                       session=session)

                compiled = compiler.compile_source(source=model, config=Config())

                self.assertEqual(len(compiled.inputs), 1)
                self.assertIs(compiled.inputs[0].tensor, model.model.input)
                self.assertEqual(compiled.inputs[0].data_format, data_format)

                self.assertEqual(len(compiled.outputs), 1)
                self.assertIs(compiled.outputs[0], model.model.output)

                self.assertIs(compiled.session, session)

    def test_compile_with_direct_layer_data_format(self):
        for (keras_data_format, data_format) in [('channels_first', DataFormat.CHANNELS_FIRST),
                                                 ('channels_last', DataFormat.CHANNELS_LAST)]:
            with self.subTest(data_format=keras_data_format):
                with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
                    input_x = keras.layers.Input(shape=(28, 28, 3), name='l0')

                    output_y = keras.layers.Conv2D(filters=4,
                                                   kernel_size=(3, 3),
                                                   data_format=keras_data_format,
                                                   name='l1')(input_x)

                model = KerasModel(model=keras.Model(inputs=[input_x], outputs=[output_y]), session=session)
                compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

                self.assertEqual(len(compiled.inputs), 1)
                self.assertIs(compiled.inputs[0].tensor, model.model.input)
                self.assertEqual(compiled.inputs[0].data_format, data_format)

                self.assertEqual(len(compiled.outputs), 1)
                self.assertIs(compiled.outputs[0], model.model.output)

                self.assertIs(compiled.session, session)

    def test_compile_with_input_layer_data_format(self):
        for (keras_data_format, data_format) in [('channels_first', DataFormat.CHANNELS_FIRST),
                                                 ('channels_last', DataFormat.CHANNELS_LAST)]:
            with self.subTest(data_format=keras_data_format):
                with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
                    input_x = keras.layers.Input(shape=(28, 28, 3), name='l0')

                    output_y = keras.layers.Conv2D(filters=4,
                                                   kernel_size=(3, 3),
                                                   data_format=keras_data_format,
                                                   name='l1')(input_x)

                model = KerasModel(model=keras.Model(inputs=[input_x], outputs=[output_y]), session=session)
                compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

                self.assertEqual(len(compiled.inputs), 1)
                self.assertIs(compiled.inputs[0].tensor, model.model.input)
                self.assertEqual(compiled.inputs[0].data_format, data_format)

                self.assertEqual(len(compiled.outputs), 1)
                self.assertIs(compiled.outputs[0], model.model.output)

                self.assertIs(compiled.session, session)

    def test_compile_with_consistent_data_format(self):
        for (keras_data_format, data_format) in [('channels_first', DataFormat.CHANNELS_FIRST),
                                                 ('channels_last', DataFormat.CHANNELS_LAST)]:
            with self.subTest(data_format=keras_data_format):
                with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
                    input_x = keras.layers.Input(shape=(28, 28, 3), name='l0')

                    output_y_1 = keras.layers.Conv2D(filters=4,
                                                     kernel_size=(3, 3),
                                                     data_format=keras_data_format,
                                                     name='l1')(input_x)

                    output_y_2 = keras.layers.Conv2D(filters=4,
                                                     kernel_size=(3, 3),
                                                     data_format=keras_data_format,
                                                     name='l2')(input_x)

                model = KerasModel(model=keras.Model(inputs=[input_x], outputs=[output_y_1, output_y_2]),
                                   session=session)

                compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

                self.assertEqual(len(compiled.inputs), 1)
                self.assertIs(compiled.inputs[0].tensor, model.model.input)
                self.assertEqual(compiled.inputs[0].data_format, data_format)

                self.assertEqual(len(compiled.outputs), 2)
                self.assertIs(compiled.outputs[0], model.model.outputs[0])
                self.assertIs(compiled.outputs[1], model.model.outputs[1])

                self.assertIs(compiled.session, session)

    def test_compile_with_conflicting_data_format_1(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = keras.layers.Input(shape=(28, 28, 3), name='l0')

            input_y_1 = keras.layers.Conv2D(filters=4,
                                            kernel_size=(3, 3),
                                            data_format='channels_first',
                                            name='l1')(input_x)

            input_y_2 = keras.layers.Conv2D(filters=4,
                                            kernel_size=(3, 3),
                                            data_format='channels_last',
                                            name='l2')(input_x)

        model = KerasModel(model=keras.Model(inputs=[input_x], outputs=[input_y_1, input_y_2]), session=session)
        compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 2)
        self.assertIs(compiled.outputs[0], model.model.outputs[0])
        self.assertIs(compiled.outputs[1], model.model.outputs[1])

        self.assertIs(compiled.session, session)

    def test_compile_with_conflicting_data_format_2(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = keras.layers.Input(shape=(28, 28, 3), name='l0')

            input_y_1 = keras.layers.Conv2D(filters=4,
                                            kernel_size=(3, 3),
                                            data_format='channels_last',
                                            name='l1')(input_x)

            input_y_2 = keras.layers.Conv2D(filters=4,
                                            kernel_size=(3, 3),
                                            data_format='channels_first',
                                            name='l2')(input_x)

        model = KerasModel(model=keras.Model(inputs=[input_x], outputs=[input_y_1, input_y_2]), session=session)
        compiled = compiler.compile_source(source=model, config=Config(input_nodes=[NodeSpec(layer_name='l0')]))

        self.assertEqual(len(compiled.inputs), 1)
        self.assertIs(compiled.inputs[0].tensor, model.model.input)
        self.assertIsNone(compiled.inputs[0].data_format)

        self.assertEqual(len(compiled.outputs), 2)
        self.assertIs(compiled.outputs[0], model.model.outputs[0])
        self.assertIs(compiled.outputs[1], model.model.outputs[1])

        self.assertIs(compiled.session, session)
