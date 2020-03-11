# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Openvino model compiler
"""

import os
import subprocess

from model_compiler import log_util
from ..compiler_base import BaseCompiler
from ..importer import import_file

_LOGGER = log_util.get_logger(__name__)


class Compiler(BaseCompiler):
    """
    OpenVINO model compiler
    """

    def __init__(self, config):
        super(Compiler, self).__init__(config)
        self.sdk_dir = os.getenv('INTEL_CVSDK_DIR', '/opt/intel/computer_vision_sdk_2018.5.455')
        self.frozen_pb_path = os.path.join(self.model_dir, 'frozen.pb')

    def _after_load_model(self, session, inputs, outputs):
        return self._to_frozen_graph(session, self.frozen_pb_path, outputs)

    def _after_end_session(self, model_info):
        _LOGGER.info("_after_end_session:: sdk install dir: %s", self.sdk_dir)
        _LOGGER.info('Start convert %s model to OpenVINO model', model_info.source_type)
        if model_info.source_type == 'ONNX':
            self._convert_model_onnx(model_info)
        else:
            self._convert_model_tf(model_info)

    def _convert_model(self, model_info, model_path):
        convert_file_path = os.path.join(self.sdk_dir, 'deployment_tools/model_optimizer/mo.py')
        if not os.path.exists(convert_file_path):
            raise Exception('mo.py does not exist, path: {}'.format(convert_file_path))
        popenargs = ['python3', convert_file_path]
        popenargs.extend(['--input_model', model_path])
        popenargs.extend(['--model_name', 'model'])
        popenargs.extend(['--output_dir', self.target_dir])
        popenargs.extend(['--batch', str(model_info.max_batch_size)])
        popenargs.extend(['--input', ','.join([i.name for i in model_info.inputs])])
        popenargs.extend(['--output', ','.join(o.name for o in model_info.outputs)])
        popenargs.extend(['--log_level', 'WARNING'])
        _LOGGER.info('Subprocess parameters: %s', popenargs)
        try:
            # add timeout, if process not complete in 10min, maybe error
            result = subprocess.run(popenargs, timeout=600, check=True, universal_newlines=True, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
            _LOGGER.info('Convert to OpenVINO model success, output: %s', result.stdout)
        except subprocess.CalledProcessError as error:
            _LOGGER.error('Convert to OpenVINO model failed, return code: %s, output: %s, stderr: %s', error.returncode,
                          error.output, error.stderr)
            raise error

    def _convert_model_onnx(self, model_info):
        model_path = self.model_path
        for i in model_info.inputs:
            if i.data_format == 'channels_last':
                raise Exception('The data format: {} is not support'.format(i.data_format))
        self._convert_model(model_info=model_info, model_path=model_path)

    def _convert_model_tf(self, model_info):
        model_path = self.frozen_pb_path
        for i in model_info.inputs:
            # OpenVINO only support NCHW, so should transpose shape if data_format is 'channels_last'
            if i.data_format == 'channels_last':
                channel = i.shape.pop(-1)
                i.shape.insert(0, channel)
                i.data_format = 'channels_first'
        self._convert_model(model_info=model_info, model_path=model_path)

    def get_platform(self):
        """
        Get platform
        :return:
        """
        version = None
        if self.sdk_dir:
            version_py_path = os.path.join(self.sdk_dir, 'deployment_tools/model_optimizer/mo/utils/version.py')
            if os.path.exists(version_py_path):
                version_module = import_file(version_py_path)
                if hasattr(version_module, "get_version") and callable(version_module.get_version):
                    version = getattr(version_module, 'get_version')()
        return "openvino", version
