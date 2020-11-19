# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from typing import NamedTuple
from unittest import TestCase

from model_compiler import serving_model_repository
from model_compiler.protos.generated.model_config_pb2 import ModelConfigProto
from model_compiler.serving_model_repository import Config


class _TestModel(NamedTuple):
    data: str

    def save(self, path: str):
        os.makedirs(path, exist_ok=True)

        with open(os.path.join(path, 'model'), 'w') as file:
            file.write(self.data)


def _read_file(path):
    with open(path) as file:
        return file.read()


class SaveModelTestCase(TestCase):
    def test_save_init(self):
        with TemporaryDirectory() as repository_dir:
            model = _TestModel(data='foobar')
            config = Config(model=model, model_config=ModelConfigProto(name='init'), path=repository_dir)
            model_dir = os.path.join(repository_dir, 'init')

            serving_model_repository.save_model(config=config)

            self.assertEqual(sorted(os.listdir(repository_dir)), ['init', 'init_1.zip'])
            self.assertEqual(sorted(os.listdir(model_dir)), ['1', 'config.pbtxt'])
            self.assertEqual(os.listdir(os.path.join(model_dir, '1')), ['model'])
            self.assertEqual(_read_file(os.path.join(model_dir, '1', 'model')), 'foobar')

    def test_save_auto_version(self):
        with TemporaryDirectory() as repository_dir:
            model = _TestModel(data='2000')
            config = Config(model=model, model_config=ModelConfigProto(name='auto-version'), path=repository_dir)
            model_dir = os.path.join(repository_dir, 'auto-version')

            os.makedirs(os.path.join(model_dir, '2'))
            os.makedirs(os.path.join(model_dir, '4'))

            serving_model_repository.save_model(config=config)

            self.assertEqual(sorted(os.listdir(repository_dir)), ['auto-version', 'auto-version_5.zip'])
            self.assertEqual(sorted(os.listdir(model_dir)), ['2', '4', '5', 'config.pbtxt'])
            self.assertEqual(os.listdir(os.path.join(model_dir, '5')), ['model'])
            self.assertEqual(_read_file(os.path.join(model_dir, '5', 'model')), '2000')

    def test_save_custom_version(self):
        with TemporaryDirectory() as repository_dir:
            model = _TestModel(data='qwerty')
            config = Config(model=model, model_config=ModelConfigProto(name='custom'), path=repository_dir, version=4)
            model_dir = os.path.join(repository_dir, 'custom')

            serving_model_repository.save_model(config=config)

            self.assertEqual(sorted(os.listdir(repository_dir)), ['custom', 'custom_4.zip'])
            self.assertEqual(sorted(os.listdir(model_dir)), ['4', 'config.pbtxt'])
            self.assertEqual(os.listdir(os.path.join(model_dir, '4')), ['model'])
            self.assertEqual(_read_file(os.path.join(model_dir, '4', 'model')), 'qwerty')

    def test_save_ignore_invalid_directory(self):
        with TemporaryDirectory() as repository_dir:
            model = _TestModel(data='foobar')
            config = Config(model=model, model_config=ModelConfigProto(name='ignore-invalid'), path=repository_dir)
            model_dir = os.path.join(repository_dir, 'ignore-invalid')

            os.makedirs(os.path.join(os.path.join(model_dir, 'not-a-number')), exist_ok=True)

            serving_model_repository.save_model(config=config)

            self.assertEqual(sorted(os.listdir(repository_dir)), ['ignore-invalid', 'ignore-invalid_1.zip'])
            self.assertEqual(sorted(os.listdir(model_dir)), ['1', 'config.pbtxt', 'not-a-number'])
            self.assertEqual(os.listdir(os.path.join(model_dir, '1')), ['model'])
            self.assertEqual(_read_file(os.path.join(model_dir, '1', 'model')), 'foobar')
