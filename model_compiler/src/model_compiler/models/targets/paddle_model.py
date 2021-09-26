# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from os.path import join
from shutil import copytree
from tempfile import TemporaryDirectory
from typing import NamedTuple, Optional, Sequence, Tuple

import paddle

from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput
from .. import repository
from ..data_format import DataFormat, as_model_config_data_format
from ..data_type import DataType


@repository.REPOSITORY.register_target_model('paddle')
class PaddleModel(NamedTuple):
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
        return 'paddle', paddle.__version__

    @staticmethod
    def from_directory(temp_path: TemporaryDirectory, input_formats: Sequence[Optional[DataFormat]]) -> 'PaddleModel':
        paddle.enable_static()
        exe = paddle.static.Executor(paddle.CPUPlace())
        inference_program, feed_var_names, fetch_vars = paddle.static.load_inference_model(
            join(temp_path.name, "model"), exe)
        feed_vars = [inference_program.global_block().vars[var_name] for var_name in feed_var_names]
        inputs = []
        outputs = []
        for i, var in enumerate(feed_vars):
            inputs.append(ModelInput(name=var.name,
                                     data_type=DataType.from_paddle_data_type(str(var.dtype)).to_tf_data_type(),
                                     format=as_model_config_data_format(input_formats[i]),
                                     dims=var.shape[1:]))

        for i, var in enumerate(fetch_vars):
            outputs.append(ModelOutput(name=var.name,
                                       data_type=DataType.from_paddle_data_type(str(var.dtype)).to_tf_data_type(),
                                       dims=var.shape[1:]))

        return PaddleModel(inputs, outputs, temp_path)
