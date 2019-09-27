"""
export model config proto
"""
import os

from google.protobuf import text_format

from ..log_util import get_logger

_LOGGER = get_logger(__name__)


class ModelProtoExporter:
    """
    Export model config proto (config.pbtxt), which use to load model when inference
    """

    def __init__(self, target_dir, model_info, platform_getter):
        self._config_path = os.path.join(target_dir, 'config.pbtxt')
        self._model = model_info
        self._platform_getter = platform_getter

    def export(self):
        """
        Export config.pbtxt, which use to do inference in servinglite
        :return:
        """
        config_proto = self._make_config_proto()
        _LOGGER.info("Export config: %s", config_proto)
        if config_proto is not None:
            with open(self._config_path, "w") as config_file:
                config_file.write(text_format.MessageToString(config_proto))
            _LOGGER.info("Save config.pbtxt success, path: %s", self._config_path)
        else:
            raise Exception("Not generate config proto, can't save config.pbtxt!")

    def _make_config_proto(self):
        """
        Export config.pbtxt, which use to do inference in servinglite
        :return:
        """
        try:
            from adlik_serving import model_config_pb2
            config_proto = model_config_pb2.ModelConfigProto()
            config_proto.name = self._model.model_name
            config_proto.platform, version = self._platform_getter()
            if version is not None:
                config_proto.platform_version = version
            config_proto.max_batch_size = self._model.max_batch_size

            for i in self._model.inputs:
                one_input = config_proto.input.add()
                one_input.name = i.name
                if i.data_format == 'channels_last':
                    one_input.format = model_config_pb2.ModelInput.FORMAT_NHWC
                elif i.data_format == 'channels_first':
                    one_input.format = model_config_pb2.ModelInput.FORMAT_NCHW
                else:
                    pass
                if hasattr(i, 'onnx_datatype_enum'):
                    one_input.data_type = i.onnx_datatype_enum
                else:
                    one_input.data_type = i.dtype.as_datatype_enum
                one_input.dims[:] = i.shape

            for raw_output in self._model.outputs:
                output = config_proto.output.add()
                output.name = raw_output.name
                if hasattr(raw_output, 'onnx_datatype_enum'):
                    output.data_type = raw_output.onnx_datatype_enum
                else:
                    output.data_type = raw_output.dtype.as_datatype_enum
                output.dims[:] = raw_output.shape
            return config_proto
        except ImportError as error:
            _LOGGER.error('No servinglite module, import error: %s', error)
            raise error
