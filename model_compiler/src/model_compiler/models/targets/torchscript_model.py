# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from shutil import copytree
from tempfile import TemporaryDirectory
from typing import NamedTuple, Sequence, Tuple
import torch

from .. import repository
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput
from ...torchscript_util import Config
from ...models.data_type import DataType
from ...models.data_format import as_model_config_data_format


@repository.REPOSITORY.register_target_model('torchscript')
class TorchscriptModel(NamedTuple):
    config: Config
    model_outputs: torch.Tensor    # pylint: disable=no-member
    temp_path: TemporaryDirectory

    def get_inputs(self) -> Sequence[ModelInput]:
        inputs = []
        for i, name in enumerate(self.config.input_names):
            inputs.append(ModelInput(name=name,
                          data_type=DataType.from_torch_data_type(self.config.data_type).to_tf_data_type(),
                          format=as_model_config_data_format(self.config.input_formats[i]),
                          dims=self.config.input_shapes[i]))
        return inputs

    def get_outputs(self) -> Sequence[ModelOutput]:
        outputs = []
        for i, output in enumerate(self.model_outputs):
            outputs.append(ModelOutput(name='output_'+str(i) if output.name is None else str(output.name),
                           data_type=DataType.from_torch_data_type(self.config.data_type).to_tf_data_type(),
                           dims=list(output.shape)))   # pylint: disable=no-member
        return outputs

    def save(self, path: str) -> None:
        copytree(src=self.temp_path.name, dst=path)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'libtorch', torch.__version__
