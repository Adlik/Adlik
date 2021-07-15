# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Mapping, NamedTuple, Optional, Sequence
import tensorflow as tf

from . import repository
from ..models.sources.saved_model_file import SavedModelFile
from ..models.targets.saved_model import SavedModel, Input, Output, DataFormat
from .. import utilities


class Config(NamedTuple):
    input_names: Optional[Sequence[str]] = None
    data_formats: Sequence[Optional[DataFormat]] = []
    output_names: Optional[Sequence[str]] = None
    signature_keys: Optional[str] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value.get('input_names'),
                      data_formats=utilities.get_data_formats(value.get('input_formats')),
                      output_names=value.get('output_names'),
                      signature_keys=value.get('signature_keys'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(input_names=utilities.split_by(env.get('INPUT_NAMES'), ','),
                      data_formats=utilities.get_data_formats(utilities.split_by('INPUT_FORMATS', ',')),
                      output_names=utilities.split_by(env.get('OUTPUT_NAMES'), ','),
                      signature_keys=env.get('SIGNATURE_KEYS'))


def get_model_info(signature):
    model_layers = {}
    if len(signature) == 1:
        model_layers = signature
    else:
        for input_signature in signature:
            if input_signature:
                model_layers.update(input_signature)

    return model_layers


def _get_inputs(model_inputs, input_names, data_formats):
    inputs = []
    if input_names:
        for i, input_name in enumerate(input_names):
            inputs.append(Input(input_name, model_inputs[input_name],
                                data_formats[i] if i < len(data_formats) else None))

    else:
        for i, input_name in enumerate(model_inputs.keys()):
            inputs.append(Input(input_name, model_inputs[input_name],
                                data_formats[i] if i < len(data_formats) else None))

    return inputs


def _get_outputs(model_outputs, output_names):
    outputs = []
    if output_names:
        for output_name in output_names:
            outputs.append(Output(output_name, model_outputs[output_name]))
    else:
        for key in model_outputs.keys():
            outputs.append(Output(key, model_outputs[key]))
    return outputs


@repository.REPOSITORY.register(source_type=SavedModelFile, target_type=SavedModel, config_type=Config)
def compile_source(source: SavedModelFile, config: Config) -> SavedModel:
    model = tf.saved_model.load(source.model_path, tags=['serve'])
    if config.signature_keys:
        infer = model.signatures[config.signature_keys]
    else:
        infer = model.signatures['serving_default']

    return SavedModel(inputs=_get_inputs(get_model_info(infer.structured_input_signature),
                                         config.input_names, config.data_formats),
                      outputs=_get_outputs(get_model_info(infer.structured_outputs), config.output_names),
                      session=None,
                      model=model,
                      signature=infer)
