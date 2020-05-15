# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple, Optional, Sequence, Tuple

import tensorflow as tf

from .. import data_format, repository
from ..data_format import DataFormat
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


class Input(NamedTuple):
    name: str
    tensor: tf.Tensor
    data_format: Optional[DataFormat] = None


class Output(NamedTuple):
    name: str
    tensor: tf.Tensor


@repository.REPOSITORY.register_target_model('tf')
class SavedModel(NamedTuple):
    inputs: Sequence[Input]
    outputs: Sequence[Output]
    session: tf.compat.v1.Session

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=item.name,
                           data_type=item.tensor.dtype.as_datatype_enum,
                           format=data_format.as_model_config_data_format(item.data_format),
                           dims=[-1 if dim is None else dim for dim in item.tensor.shape[1:]])
                for item in self.inputs]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=item.name,
                            data_type=item.tensor.dtype.as_datatype_enum,
                            dims=[-1 if dim is None else dim for dim in item.tensor.shape[1:]])
                for item in self.outputs]

    def save(self, path: str) -> None:
        with self.session.graph.as_default():
            builder = tf.compat.v1.saved_model.Builder(export_dir=path)

            builder.add_meta_graph_and_variables(
                sess=self.session,
                tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
                signature_def_map={
                    'predict': tf.compat.v1.saved_model.predict_signature_def(inputs={item.name: item.tensor
                                                                                      for item in self.inputs},
                                                                              outputs={item.name: item.tensor
                                                                                       for item in self.outputs})
                },
                clear_devices=True
            )

            builder.save()

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'tensorflow', tf.version.VERSION
