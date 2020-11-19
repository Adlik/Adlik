# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple, Optional, Sequence

from onnx import ModelProto

from ..data_format import DataFormat


class OnnxModel(NamedTuple):
    model_proto: ModelProto
    input_data_formats: Sequence[Optional[DataFormat]]

    def get_inputs(self):
        graph = self.model_proto.graph
        initializers = {initializer.name for initializer in graph.initializer}

        return [input_spec for input_spec in graph.input if input_spec.name not in initializers]
