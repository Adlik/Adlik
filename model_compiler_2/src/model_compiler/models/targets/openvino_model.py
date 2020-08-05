# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from distutils.dir_util import copy_tree
from tempfile import TemporaryDirectory
from typing import NamedTuple, Sequence, Tuple

from .. import repository
from ...openvino_util import get_version
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


@repository.REPOSITORY.register_target_model('openvino')
class OpenvinoModel(NamedTuple):
    inputs: Sequence[ModelInput]
    outputs: Sequence[ModelOutput]
    temp_path: TemporaryDirectory

    def get_inputs(self) -> Sequence[ModelInput]:
        return self.inputs

    def get_outputs(self) -> Sequence[ModelOutput]:
        return self.outputs

    def save(self, path: str) -> None:
        copy_tree(self.temp_path.name, path)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'openvino', get_version()
