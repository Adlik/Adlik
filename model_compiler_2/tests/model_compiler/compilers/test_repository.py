# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple
from unittest import TestCase

from model_compiler.compilers.repository import Repository


class RepositoryTestCase(TestCase):
    def test_get_identity(self):
        repository = Repository()
        compiler, config_type = repository.get(int, int)

        self.assertEqual(compiler(7, config_type.from_json({})), 7)

    def test_get_single_simple_step(self):
        repository = Repository()

        def _int_to_float(source: int) -> float:
            return float(source)

        repository.register(source_type=int, target_type=float)(_int_to_float)

        compiler, config_type = repository.get(int, float)

        result = compiler(7, config_type.from_json({}))

        self.assertIsInstance(result, float)
        self.assertEqual(result, 7.0)

    def test_find_short_path(self):
        repository = Repository()

        def _float_to_list(_source: float) -> list:
            pass  # pragma: no cover

        repository.register(int, float)(float)
        repository.register(float, list)(_float_to_list)
        repository.register(int, list)(lambda value: [value, value])

        compiler, config_type = repository.get(int, list)

        self.assertEqual(compiler(4, config_type.from_json({})), [4, 4])

    def test_path_not_found(self):
        repository = Repository()

        def _int_to_float(_source: int) -> float:
            pass  # pragma: no cover

        repository.register(source_type=int, target_type=float)(_int_to_float)

        with self.assertRaises(ValueError):
            repository.get(float, int)

    def test_get_single_step_with_configuration(self):
        repository = Repository()

        class _Config(NamedTuple):
            x: int

            @staticmethod
            def from_json(value) -> '_Config':
                return _Config(value['x'])

            @staticmethod
            def from_env(env) -> '_Config':
                return _Config(env['X'])

        def _int_to_float(source: int, config: _Config) -> float:
            return float(source * config.x)

        repository.register(source_type=int, target_type=float, config_type=_Config)(_int_to_float)

        compiler, config_type = repository.get(int, float)
        result = compiler(7, config_type.from_json({'x': 8}))

        self.assertIsInstance(result, float)
        self.assertEqual(result, 56.0)

        result = compiler(7, config_type.from_env({'X': 8}))

        self.assertIsInstance(result, float)
        self.assertEqual(result, 56.0)

    def test_get_multiple_steps_with_configuration(self):
        repository = Repository()

        class _Config1(NamedTuple):
            x: int

            @staticmethod
            def from_json(value):
                return _Config1(x=value['x'])

            @staticmethod
            def from_env(env):
                return _Config1(x=env['X'])

        class _Config2(NamedTuple):
            y: int

            @staticmethod
            def from_json(value):
                return _Config2(y=value['y'])

            @staticmethod
            def from_env(env):
                return _Config2(y=env['Y'])

        def _int_to_float(source: int, config: _Config1) -> float:
            return float(source * config.x)

        def _float_to_list(source: float, config: _Config2) -> list:
            return [source, config.y]

        repository.register(source_type=int, target_type=float, config_type=_Config1)(_int_to_float)
        repository.register(source_type=float, target_type=list, config_type=_Config2)(_float_to_list)

        compiler, config_type = repository.get(int, list)
        result = compiler(7, config_type.from_json({'x': 8, 'y': 3}))

        self.assertEqual(result, [56.0, 3])

        result = compiler(7, config_type.from_env({'X': 8, 'Y': 3}))

        self.assertEqual(result, [56.0, 3])
