"""
Serving model compiler
"""

import os

import tensorflow as tf

from model_compiler import log_util
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
            if not isinstance(config.get_attribute(item), list):
                _LOGGER.error('Require "%s" but not found or type is invalid', item)
                raise Exception('Require {} but not found or type is invalid'.format(item))
            self.__setattr__(item, config.get_attribute(item))
        if len(self.input_signatures) != len(self.input_layer_names) and len(self.input_signatures) != len(
                self.input_names):
            raise Exception('input signatures number is not equal to inputs')

        if len(self.output_signatures) != len(self.output_layer_names) and len(self.output_signatures) != len(
                self.output_names):
            raise Exception('output signatures number is not equal to outputs')

    def _after_load_model(self, session, inputs, outputs):
        """
        Compile serving model
        :return: Return compile result
        """
        _LOGGER.info('Start to compile serving model')
        inputs_dict = {sign: tensor for (sign, tensor) in
                       zip(self.input_signatures, [i.tensor for i in inputs])}
        outputs_dict = {sign: tensor for (sign, tensor) in
                        zip(self.output_signatures, [o.tensor for o in outputs])}
        _LOGGER.info('Serving model signature info: inputs: %s, outputs: %s', inputs_dict, outputs_dict)

        signature = tf.saved_model.signature_def_utils.predict_signature_def(inputs=inputs_dict,
                                                                             outputs=outputs_dict)
        _LOGGER.info('Create signature def success')
        builder = tf.saved_model.builder.SavedModelBuilder(self.version_dir)
        builder.add_meta_graph_and_variables(sess=session, tags=[tf.saved_model.tag_constants.SERVING],
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
