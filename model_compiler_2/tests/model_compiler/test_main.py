# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess  # nosec
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import keras
import tensorflow as tf


def _save_model(k, path):
    with tf.compat.v1.Session(graph=tf.Graph()):
        k.Sequential([k.layers.Dense(units=4, input_shape=[8])]).save(path)


class MainTestCase(TestCase):
    def test_compile_from_json(self):
        for k in [keras, tf.keras]:
            with self.subTest(k=k), \
                 NamedTemporaryFile(suffix='.h5') as model_file, \
                    NamedTemporaryFile(mode='w') as config_file, \
                    TemporaryDirectory() as target_dir:
                _save_model(k, model_file.name)

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
        for k in [keras, tf.keras]:
            with self.subTest(k=k), NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
                _save_model(k, model_file.name)

                env = os.environ.copy()

                env.update({'serving_type': 'tf',
                            'model_name': 'foobar2000',
                            'version': '4',
                            'max_batch_size': '7',
                            'h5_path': model_file.name,
                            'input_signatures': 'x_2000',
                            'output_signatures': 'y_2000',
                            'export_path': target_dir})

                subprocess.run(args=['model-compiler'], env=env, check=True)  # nosec

                self.assertEqual(sorted(os.listdir(target_dir)), ['foobar2000', 'foobar2000_4.zip'])
                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000'))), ['4', 'config.pbtxt'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4'))),
                                 ['saved_model.pb', 'variables'])

                self.assertEqual(sorted(os.listdir(os.path.join(target_dir, 'foobar2000', '4', 'variables'))),
                                 ['variables.data-00000-of-00001', 'variables.index'])
