# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Serving model compiler
"""

import os

from model_compiler import log_util
import tensorflow as tf

from ..compiler_base import BaseCompiler

_LOGGER = log_util.get_logger(__name__)


class Compiler(BaseCompiler):
    """
    Serving model compiler
    """

    _REQUIRED = ["input_signatures",
                 "output_signatures"]

    def __init__(self, config):
        super(Compiler, self).__init__(config)
        for item in self._REQUIRED:
            self.__setattr__(item, config.get_attribute(item, []))

    def _normalize_config(self, inputs, outputs):
        if len(self.input_signatures) < len(inputs):
            for i in range(len(self.input_signatures), len(inputs)):
                self.input_signatures.append(inputs[i].name)

        if len(self.output_signatures) < len(outputs):
            for i in range(len(self.output_signatures), len(outputs)):
                self.output_signatures.append(outputs[i].name)

    def _after_load_model(self, session, inputs, outputs):
        """
        Compile serving model
        :return: Return compile result
        """
        _LOGGER.info('Start to compile serving model')
        self._normalize_config(inputs, outputs)
        inputs_dict = {sign: tensor for (sign, tensor) in
                       zip(self.input_signatures, [i.tensor for i in inputs])}
        outputs_dict = {sign: tensor for (sign, tensor) in
                        zip(self.output_signatures, [o.tensor for o in outputs])}
        _LOGGER.info('Serving model signature info: inputs: %s, outputs: %s', inputs_dict, outputs_dict)

        signature = tf.compat.v1.saved_model.signature_def_utils.predict_signature_def(inputs=inputs_dict,
                                                                                       outputs=outputs_dict)
        _LOGGER.info('Create signature def success')
        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(self.version_dir)
        builder.add_meta_graph_and_variables(sess=session, tags=[tf.saved_model.SERVING],
                                             signature_def_map={'predict': signature}, clear_devices=True)

        serving_path = builder.save(True)
        _LOGGER.info('Saved model success, serving path: %s', serving_path)
        if os.path.isfile(serving_path):
            self._write_version_file(serving_path)

        # replace input and output op names by signatures
        self._update_names_by_signature(inputs, outputs)

    def _update_names_by_signature(self, inputs, outputs):
        for (i, signature) in zip(inputs, self.input_signatures):
            i.name = signature
        for (output, signature) in zip(outputs, self.output_signatures):
            output.name = signature

    def _after_end_session(self, model_info):
        pass

    @staticmethod
    def _write_version_file(serving_path):
        version_path = os.path.dirname(serving_path)
        if isinstance(version_path, bytes):
            version_path = version_path.decode('utf-8')
        with open(os.path.join(str(version_path), "TFVERSION"), 'w') as version_file:
            version_file.write(tf.__version__)
        _LOGGER.info('Write TFVERSION file success, path: %s', version_path)

    def get_platform(self):
        """
        Get platform
        :return:
        """
        return "tensorflow", tf.__version__
