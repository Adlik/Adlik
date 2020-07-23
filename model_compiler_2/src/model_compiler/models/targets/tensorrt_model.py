# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Any, NamedTuple, Optional, Sequence, Tuple

from .. import data_format, repository
from ..data_format import DataFormat
from ..data_type import DataType
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _trt_dtype_to_tf_dtype(trt_dtype):
    return DataType.from_tensorrt_data_type(trt_dtype).to_tf_data_type()


@repository.REPOSITORY.register_target_model('tensorrt')
class TensorRTModel(NamedTuple):
    cuda_engine: Any  # ICudaEngine
    input_data_formats: Sequence[Optional[DataFormat]]

    def _get_binding_shape(self, i):
        cuda_engine = self.cuda_engine

        if cuda_engine.has_implicit_batch_dimension:
            return cuda_engine.get_binding_shape(i)

        return cuda_engine.get_binding_shape(i)[1:]

    def get_inputs(self) -> Sequence[ModelInput]:
        input_binding_indices = filter(self.cuda_engine.binding_is_input, range(self.cuda_engine.num_bindings))

        return [ModelInput(name=self.cuda_engine.get_binding_name(i),
                           data_type=_trt_dtype_to_tf_dtype(self.cuda_engine.get_binding_dtype(i)),
                           format=data_format.as_model_config_data_format(input_format),
                           dims=self._get_binding_shape(i))
                for i, input_format in zip(input_binding_indices, self.input_data_formats)]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=self.cuda_engine.get_binding_name(i),
                            data_type=_trt_dtype_to_tf_dtype(self.cuda_engine.get_binding_dtype(i)),
                            dims=self._get_binding_shape(i))
                for i in range(self.cuda_engine.num_bindings)
                if not self.cuda_engine.binding_is_input(i)]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        with open(os.path.join(path, 'model.plan'), 'wb') as target:
            with self.cuda_engine.serialize() as data:
                target.write(data)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        import tensorrt  # pylint: disable=import-outside-toplevel

        return 'tensorrt_plan', tensorrt.__version__
