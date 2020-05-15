from os import path
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import keras
import tensorflow as tf

import model_compiler


class CompileModelTestCase(TestCase):
    def test_error_result(self):
        result = model_compiler.compile_model({})

        self.assertEqual(result, {'status': 'failure', 'error_msg': 'Unable to determine the source model type.'})

    def test_success_result(self):
        for k in [keras, tf.keras]:
            with self.subTest(k=k), NamedTemporaryFile(suffix='.h5') as model_file, TemporaryDirectory() as target_dir:
                with tf.compat.v1.Session(graph=tf.Graph()):
                    k.Sequential([k.layers.Dense(units=4, input_shape=[8])]).save(model_file.name)

                result = model_compiler.compile_model({
                    'serving_type': 'tf',
                    'model_name': 'foobar',
                    'version': 4,
                    'max_batch_size': 7,
                    'input_model': model_file.name,
                    'input_signatures': ['x'],
                    'output_signatures': ['y'],
                    'export_path': target_dir
                })

                self.assertEqual(result, {'status': 'success', 'path': path.join(target_dir, 'foobar_4.zip')})
