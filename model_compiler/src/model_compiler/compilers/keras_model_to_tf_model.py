# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from . import repository
from .. import utilities
from ..models.irs.keras_model import KerasModel
from ..models.irs.tf_model import Input, TensorFlowModel
from ..keras_util import Config, get_inputs, get_outputs


@repository.REPOSITORY.register(source_type=KerasModel, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: KerasModel, config: Config) -> TensorFlowModel:
    inputs = [Input(tensor=tensor, data_format=data_format)
              for tensor, data_format in get_inputs(source.model, config.input_nodes)]
    outputs = get_outputs(source.model, config.output_nodes)
    utilities.judge_batch_size([model_input.tensor.shape for model_input in inputs],
                               [model_output.shape for model_output in outputs])

    return TensorFlowModel(inputs=inputs, outputs=outputs, session=source.session)
