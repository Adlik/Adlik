# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Any, NamedTuple, Optional, Sequence, Tuple
import tvm

from .. import data_format, repository
from ..data_format import DataFormat
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


class Input(NamedTuple):
    name: str
    shape: list
    data_type: Any
    data_format: Optional[DataFormat] = None


class Output(NamedTuple):
    name: str
    shape: list
    data_type: Any


@repository.REPOSITORY.register_target_model('tvm')
class TvmModel(NamedTuple):
    tvm_model: tvm.relay.backend.graph_runtime_factory.GraphRuntimeFactoryModule
    model_inputs: Sequence[Input]
    model_outputs: Sequence[Output]

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=name,
                           data_type=data_type,
                           format=data_format.as_model_config_data_format(input_format),
                           dims=[-1 if dim is None else dim for dim in shape[1:]])
                for name, shape, data_type, input_format in self.model_inputs]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=name,
                            data_type=data_type,
                            dims=[-1 if dim is None else dim for dim in shape[1:]])
                for name, shape, data_type in self.model_outputs]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        with open(os.path.join(path, 'model.so'), 'wb') as file:
            self.tvm_model.export_library(file.name)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'tvm', tvm.__version__
