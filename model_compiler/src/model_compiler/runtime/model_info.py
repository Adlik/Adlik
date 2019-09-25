"""
Model loader, loader model into graph
"""
from ..log_util import get_logger

_LOGGER = get_logger(__name__)


class ModelInfo:
    """
    Model information description
    """

    def __init__(self, model_name, max_batch_size, source_type):
        self.model_name = model_name
        self.max_batch_size = max_batch_size
        self.inputs = []
        self.outputs = []
        self.custom_object = None
        self.source_type = source_type

    def update(self, custom_object, inputs, outputs):
        """
        Update model inputs, outputs, custom objects
        :param custom_object:
        :param inputs:
        :param outputs:
        :return:
        """
        self.custom_object = custom_object
        self.inputs = inputs
        self.outputs = outputs
        _LOGGER.info('Update model info, custom object: %s, inputs: %s, outputs: %s', custom_object, inputs, outputs)
