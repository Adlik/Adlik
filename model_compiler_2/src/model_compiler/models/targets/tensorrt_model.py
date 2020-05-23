# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import NamedTuple, Optional, Sequence, Tuple

import tensorrt
from tensorrt import ICudaEngine

from .. import data_format, repository
from ..data_format import DataFormat
from ..data_type import DataType
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


class Input(NamedTuple):
    name: str
    data_type: DataType
    shape: Sequence[int]
    data_format: Optional[DataFormat] = None


class Output(NamedTuple):
    name: str
    data_type: DataType
    shape: Sequence[int]


@repository.REPOSITORY.register_target_model('tensorrt')
class TensorRTModel(NamedTuple):
    inputs: Sequence[Input]
    outputs: Sequence[Output]
    cuda_engine: ICudaEngine

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=item.name,
                           data_type=item.data_type.to_tf_data_type(),
                           format=data_format.as_model_config_data_format(item.data_format),
                           dims=item.shape)
                for item in self.inputs]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=item.name,
                            data_type=item.data_type.to_tf_data_type(),
                            dims=item.shape)
                for item in self.outputs]

    def save(self, path: str) -> None:
        with open(os.path.join(path, 'model.plan'), 'wb') as target:
            with self.cuda_engine.serialize() as data:
                target.write(data)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'tensorrt_plan', tensorrt.__version__
