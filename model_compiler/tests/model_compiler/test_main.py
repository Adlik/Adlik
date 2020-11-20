# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess  # nosec
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import tensorflow as tf
from tensorflow import keras


def _save_model(path):
    with tf.compat.v1.Session(graph=tf.Graph()):
        keras.Sequential([keras.layers.Dense(units=4, input_shape=[8])]).save(path)


class MainTestCase(TestCase):
    def test_compile_from_json(self):
        with NamedTemporaryFile(suffix='.h5') as model_file, \
                NamedTemporaryFile(mode='w') as config_file, \
                TemporaryDirectory() as target_dir:
            _save_model(model_file.name)

            json.dump({'serving_type': 'tf',
                       'model_name': 'foobar2000',
                       'version': 4,
                       'max_batch_size': 7,
                       'input_model': model_file.name,
                       'input_signatures': ['x_2000'],
                       'output_signatures': ['y_2000'],
                       'export_path': target_dir},
                      config_file)

            config_file.flush()

            subprocess.run(args=['model-compiler', '-s', 'json', '-p', config_file.name], check=True)  # nosec

            self.assertEqual(sorted(os.listdir(target_dir)), ['foobar2000', 'foobar2000_4.zip'])
            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000'))), ['4', 'config.pbtxt'])

            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4'))),
                             ['saved_model.pb', 'variables'])

            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4', 'variables'))),
                             ['variables.data-00000-of-00001', 'variables.index'])

    def test_compile_from_env(self):
        with NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
            _save_model(model_file.name)

            env = os.environ.copy()

            env.update({'SERVING_TYPE': 'tf',
                        'MODEL_NAME': 'foobar2000',
                        'VERSION': '4',
                        'MAX_BATCH_SIZE': '7',
                        'H5_PATH': model_file.name,
                        'INPUT_SIGNATURES': 'x_2000',
                        'OUTPUT_SIGNATURES': 'y_2000',
                        'EXPORT_PATH': target_dir,
                        'CALLBACK': 'http:response'})

            subprocess.run(args=['model-compiler'], env=env, check=True)  # nosec

            self.assertEqual(sorted(os.listdir(target_dir)), ['foobar2000', 'foobar2000_4.zip'])
            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000'))), ['4', 'config.pbtxt'])

            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4'))),
                             ['saved_model.pb', 'variables'])

            self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4', 'variables'))),
                             ['variables.data-00000-of-00001', 'variables.index'])

    @staticmethod
    def test_compile_error():
        with NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
            _save_model(model_file.name)

            env = os.environ.copy()

            env.update({'SERVING_TYPE': 'tf',
                        'VERSION': '4',
                        'MAX_BATCH_SIZE': '7',
                        'H5_PATH': model_file.name,
                        'INPUT_SIGNATURES': 'x_2000',
                        'OUTPUT_SIGNATURES': 'y_2000',
                        'EXPORT_PATH': target_dir,
                        'CALLBACK': 'http://response'})

            subprocess.run(args=['model-compiler'], env=env, check=True)  # nosec

    @staticmethod
    def test_compile_error_no_callback():
        with NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
            _save_model(model_file.name)

            env = os.environ.copy()

            env.update({'SERVING_TYPE': 'tf',
                        'VERSION': '4',
                        'MAX_BATCH_SIZE': '7',
                        'H5_PATH': model_file.name,
                        'INPUT_SIGNATURES': 'x_2000',
                        'OUTPUT_SIGNATURES': 'y_2000',
                        'EXPORT_PATH': target_dir})

            subprocess.run(args=['model-compiler'], env=env, check=True)  # nosec
