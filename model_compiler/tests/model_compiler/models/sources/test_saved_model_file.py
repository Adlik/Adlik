# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.saved_model_file import SavedModelFile


class SavedModelFileTestCase(TestCase):
    def test_from_json(self):
        config = SavedModelFile.from_json({'input_model': 'foo'})
        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = SavedModelFile.from_env({'SAVED_MODEL_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')
