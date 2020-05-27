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
    data_format: Optional[DataFormat] = None


def _trt_dtype_to_tf_dtype(trt_dtype):
    return DataType.from_tensorrt_data_type(trt_dtype).to_tf_data_type()


@repository.REPOSITORY.register_target_model('tensorrt')
class TensorRTModel(NamedTuple):
    inputs: Sequence[Input]
    outputs: Sequence[str]
    cuda_engine: ICudaEngine

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=item.name,
                           data_type=_trt_dtype_to_tf_dtype(self.cuda_engine.get_binding_dtype(item.name)),
                           format=data_format.as_model_config_data_format(item.data_format),
                           dims=self.cuda_engine.get_binding_shape(item.name)[1:])
                for item in self.inputs]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=output_name,
                            data_type=_trt_dtype_to_tf_dtype(self.cuda_engine.get_binding_dtype(output_name)),
                            dims=self.cuda_engine.get_binding_shape(output_name)[1:])
                for output_name in self.outputs]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        with open(os.path.join(path, 'model.plan'), 'wb') as target:
            with self.cuda_engine.serialize() as data:
                target.write(data)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'tensorrt_plan', tensorrt.__version__
