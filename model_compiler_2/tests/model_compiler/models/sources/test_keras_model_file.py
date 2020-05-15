from unittest import TestCase

from model_compiler.models.sources.keras_model_file import KerasModelFile


class KerasModelFileTestCase(TestCase):
    def test_from_json_minimal(self):
        config = KerasModelFile.from_json({'input_model': 'foo'})

        self.assertEqual(config.model_path, 'foo')
        self.assertIsNone(config.script_path)

    def test_from_json_full(self):
        config = KerasModelFile.from_json({'input_model': 'foo', 'script_path': 'bar'})

        self.assertEqual(config.model_path, 'foo')
        self.assertEqual(config.script_path, 'bar')
