# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from collections import deque
from typing import Any, Callable, Deque, Dict, List, Mapping, NamedTuple, Optional, Sequence, Tuple, Type   # noqa: F401

from .. import utilities


class _NoConfig(NamedTuple):
    @staticmethod
    def from_json(_value) -> '_NoConfig':
        return _NoConfig()

    @staticmethod
    def from_env(_env) -> '_NoConfig':
        return _NoConfig()


class _Edge(NamedTuple):
    compiler: Callable[[Any, Any], Any]
    config_type: Type[NamedTuple]
    target_type: type

    @staticmethod
    def create(compiler, target_type, config_type=None):
        if config_type is None:
            def _compile(source, _config: _NoConfig):
                return compiler(source)

            return _Edge(compiler=_compile, config_type=_NoConfig, target_type=target_type)

        return _Edge(compiler=compiler, config_type=config_type, target_type=target_type)


def _find_path(compiler_graph: Mapping[type, List[_Edge]], source_type: type, target_type: type):
    queue = deque()  # type: Deque[Tuple[type, List[_Edge]]]
    visited = {source_type}
    current = source_type, []  # type: Tuple[type, List[_Edge]]

    while True:
        if current[0] == target_type:
            return current[1]

        edges = compiler_graph.get(current[0])

        if edges is not None:
            for edge in edges:
                if edge.target_type not in visited:
                    visited.add(edge.target_type)
                    queue.append((edge.target_type, current[1] + [edge]))

        try:
            current = queue.popleft()
        except IndexError:
            break

    raise ValueError('Path not found')


def _get_config_type(edges: Sequence[_Edge]):
    config_types = [edge.config_type for edge in edges]
    from_jsons = [getattr(config_type, 'from_json') for config_type in config_types]
    from_envs = [getattr(config_type, 'from_env') for config_type in config_types]

    class _Config(NamedTuple):
        configs: Sequence[Any]

        @staticmethod
        def from_kwargs(**kwargs) -> '_Config':
            return _Config(configs=[utilities.create_named_tuple(config_type, kwargs) for config_type in config_types])

        @staticmethod
        def from_json(value) -> '_Config':
            return _Config(configs=[func(value) for func in from_jsons])

        @staticmethod
        def from_env(env) -> '_Config':
            return _Config(configs=[func(env) for func in from_envs])

    return _Config


class Repository:
    def __init__(self):
        self._compiler_graph: Dict[type, List[_Edge]] = {}

    def register(self, source_type, target_type, config_type: Optional[type] = None):
        if not (config_type is None or (issubclass(config_type, tuple) and hasattr(config_type, '_fields'))):
            raise AssertionError

        def _decorator(compiler):
            self._compiler_graph.setdefault(source_type, []).append(_Edge.create(compiler=compiler,
                                                                                 target_type=target_type,
                                                                                 config_type=config_type))

            return compiler

        return _decorator

    def get(self, source_type, target_type):
        path = _find_path(self._compiler_graph, source_type, target_type)
        config_type = _get_config_type(path)

        def _compiler(source, config):
            print(f'Source type: {source_type.__name__}.')
            print(f'Target type: {target_type.__name__}.')

            print('Compile path: {}.'.format(
                ' -> '.join([source_type.__name__] + [edge.target_type.__name__ for edge in path])
            ))

            result = source

            for edge, inner_config in zip(path, config.configs):
                print(f'Compiling to {edge.target_type.__name__}...')

                result = edge.compiler(result, inner_config)

            return result

        return _compiler, config_type


REPOSITORY = Repository()
