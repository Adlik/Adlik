# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import contextlib
import os
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import keras
import tensorflow as tf

from model_compiler import compiler


def _save_model(k, path):
    with tf.compat.v1.Session(graph=tf.Graph()):
        k.Sequential([k.layers.Dense(units=4, input_shape=[8])]).save(path)


class CompileFromJsonTestCase(TestCase):
    def test_invalid_source(self):
        with self.assertRaises(ValueError):
            compiler.compile_from_json({})

    def test_keras_model_to_saved_model(self):
        for k in [keras, tf.keras]:
            with self.subTest(k=k), NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
                _save_model(k=k, path=model_file.name)

                compiler.compile_from_json({
                    'serving_type': 'tf',
                    'model_name': 'foobar',
                    'version': 4,
                    'max_batch_size': 7,
                    'input_model': model_file.name,
                    'input_signatures': ['x'],
                    'output_signatures': ['y'],
                    'export_path': target_dir
                })

                self.assertEqual(sorted(os.listdir(target_dir)), ['foobar', 'foobar_4.zip'])
                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar'))), ['4', 'config.pbtxt'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar', '4'))),
                                 ['saved_model.pb', 'variables'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar', '4', 'variables'))),
                                 ['variables.data-00000-of-00001', 'variables.index'])


@contextlib.contextmanager
def _use_env(env):
    environ = os.environ
    restore_actions = []

    for key, value in env.items():
        restore_actions.append((key, environ.get(key)))
        environ[key] = value

    try:
        yield
    finally:
        for key, value in reversed(restore_actions):
            if value is None:
                environ.pop(key)
            else:
                environ[key] = value


class UseEnvTestCase(TestCase):
    def test_insert(self):
        saved_env = os.environ.copy()

        self.assertNotIn('_SOME_ENV', saved_env)

        with _use_env({'_SOME_ENV': 'aabbc'}):
            self.assertEqual(os.environ['_SOME_ENV'], 'aabbc')

        self.assertNotIn('_SOME_ENV', saved_env)

    def test_replace(self):
        saved_env = os.environ.copy()

        self.assertNotIn('_SOME_ENV', saved_env)

        with _use_env({'_SOME_ENV': 'aabbc'}):
            self.assertEqual(os.environ['_SOME_ENV'], 'aabbc')

            with _use_env({'_SOME_ENV': 'qwerty'}):
                self.assertEqual(os.environ['_SOME_ENV'], 'qwerty')

            self.assertEqual(os.environ['_SOME_ENV'], 'aabbc')

        self.assertNotIn('_SOME_ENV', saved_env)


class CompileFromEnvTestCase(TestCase):
    def test_invalid_source(self):
        with self.assertRaises(ValueError):
            compiler.compile_from_env()

    def test_keras_model_to_saved_model(self):
        for k in [keras, tf.keras]:
            with self.subTest(k=k), NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
                _save_model(k=k, path=model_file.name)

                with _use_env({'serving_type': 'tf',
                               'model_name': 'foobar',
                               'version': '4',
                               'max_batch_size': '7',
                               'h5_path': model_file.name,
                               'input_signatures': 'x',
                               'output_signatures': 'y',
                               'export_path': target_dir}):
                    compiler.compile_from_env()

                self.assertEqual(sorted(os.listdir(target_dir)), ['foobar', 'foobar_4.zip'])
                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar'))), ['4', 'config.pbtxt'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar', '4'))),
                                 ['saved_model.pb', 'variables'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar', '4', 'variables'))),
                                 ['variables.data-00000-of-00001', 'variables.index'])
