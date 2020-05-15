from typing import Any, List, Mapping, NamedTuple, Optional, Sequence

from . import repository
from .. import utilities
from ..models.irs.tf_model import TensorFlowModel
from ..models.targets.saved_model import Input, Output, SavedModel


def _split_by_comma(value: Optional[str]) -> Optional[List[str]]:
    return utilities.map_optional(value, lambda val: val.split(','))


class Config(NamedTuple):
    input_signature: Optional[Sequence[str]] = None
    output_signature: Optional[Sequence[str]] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_signature=value.get('input_signatures'), output_signature=value.get('output_signatures'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_signature=_split_by_comma(env.get('input_signatures')),
                      output_signature=_split_by_comma(env.get('output_signatures')))


@repository.REPOSITORY.register(source_type=TensorFlowModel, target_type=SavedModel, config_type=Config)
def compile_source(source: TensorFlowModel, config: Config) -> SavedModel:
    if config.input_signature is None:
        inputs = [Input(name=i.tensor.name, tensor=i.tensor, data_format=i.data_format) for i in source.inputs]
    else:
        inputs = [Input(name=name, tensor=i.tensor, data_format=i.data_format)
                  for (name, i) in zip(config.input_signature, source.inputs)]

    if config.output_signature is None:
        outputs = [Output(name=tensor.name, tensor=tensor) for tensor in source.outputs]
    else:
        outputs = [Output(name=name, tensor=tensor) for (name, tensor) in zip(config.output_signature, source.outputs)]

    return SavedModel(inputs=inputs, outputs=outputs, session=source.session)
