# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from shutil import copytree
from tempfile import TemporaryDirectory
from typing import NamedTuple, Sequence, Tuple

from .. import repository
from ...openvino_util import ModelParser, get_version
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
        copytree(src=self.temp_path.name, dst=path)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'openvino', get_version()

    @staticmethod
    def from_directory(temp_path: TemporaryDirectory) -> 'OpenvinoModel':
        model_parser = ModelParser.from_xml(os.path.join(temp_path.name, 'model.xml'))
        return OpenvinoModel(model_parser.get_inputs(), model_parser.get_outputs(), temp_path)
