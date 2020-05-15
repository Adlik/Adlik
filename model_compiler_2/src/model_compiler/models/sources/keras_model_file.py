from os import path
from typing import Any, Mapping, NamedTuple, Optional

from .. import repository


@repository.REPOSITORY.register_source_model
class KerasModelFile(NamedTuple):
    model_path: str
    script_path: Optional[str] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'KerasModelFile':
        return KerasModelFile(model_path=value['input_model'], script_path=value.get('script_path'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'KerasModelFile':
        return KerasModelFile(model_path=env['h5_path'], script_path=env.get('script_path'))

    @staticmethod
    def accepts_json(value: Mapping[str, Any]) -> bool:
        input_model = value.get('input_model')

        return isinstance(input_model, str) and input_model[-3:].upper() == '.H5' and path.isfile(input_model)

    @staticmethod
    def accepts_env(env: Mapping[str, str]) -> bool:
        input_model = env.get('h5_path')

        return isinstance(input_model, str) and path.isfile(input_model)
