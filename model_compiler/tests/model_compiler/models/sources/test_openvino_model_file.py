# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import shutil
from tempfile import TemporaryDirectory, NamedTemporaryFile
from unittest import TestCase

from model_compiler.models.sources.openvino_model_file import OpenvinoModelFile


class OpenvinoModelFileTestCase(TestCase):
    def test_from_json_minimal(self):
        config = OpenvinoModelFile.from_json({'input_model': 'foo'})

        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = OpenvinoModelFile.from_env({'OPENVINO_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')

    def test_accepts_json_model_dir(self):
        with TemporaryDirectory() as model_dir:
            temp_bin = NamedTemporaryFile(suffix='.bin')
            temp_xml = NamedTemporaryFile(suffix='.xml')
            shutil.copy(temp_bin.name, model_dir)
            shutil.copy(temp_xml.name, model_dir)
            self.assertTrue(OpenvinoModelFile.accepts_json({'input_model': model_dir}))

    def test_accepts_json_non_model_dir(self):
        with TemporaryDirectory() as non_model_dir:
            temp_bin = NamedTemporaryFile(suffix='.xxx')
            temp_xml = NamedTemporaryFile(suffix='.yyy')
            shutil.copy(temp_bin.name, non_model_dir)
            shutil.copy(temp_xml.name, non_model_dir)
            self.assertFalse(OpenvinoModelFile.accepts_json({'input_model': non_model_dir}))
