# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import NamedTuple, Optional, Sequence, Tuple

import tensorflow as tf

from .. import data_format, repository
from ..data_format import DataFormat
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


@repository.REPOSITORY.register_target_model('tflite')
class TfLiteModel(NamedTuple):
    tflite_model: bytes
    input_formats: Sequence[Optional[DataFormat]]

    def _get_interpreter(self):
        return tf.lite.Interpreter(model_content=self.tflite_model)

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=model_input['name'],
                           data_type=tf.as_dtype(model_input['dtype']).as_datatype_enum,
                           format=data_format.as_model_config_data_format(input_format),
                           dims=[-1 if dim is None else dim for dim in model_input['shape'][1:]])
                for model_input, input_format in zip(self._get_interpreter().get_input_details(), self.input_formats)]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=model_output['name'],
                            data_type=tf.as_dtype(model_output['dtype']).as_datatype_enum,
                            dims=[-1 if dim is None else dim for dim in model_output['shape'][1:]])
                for model_output in self._get_interpreter().get_output_details()]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        with open(os.path.join(path, 'model.tflite'), 'wb') as file:
            file.write(self.tflite_model)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'tensorflow_lite', tf.version.VERSION
