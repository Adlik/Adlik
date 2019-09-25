"""
This is a mnist sample which can compile keras h5 model to tf serving/openvino/tensorrt model
"""
import json
import os

from model_compiler import log_util
from model_compiler.config import create_config
from model_compiler.runtime import create_compiler

_LOGGER = log_util.get_logger(__file__)


def _main():
    log_util.setup_logger()
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "tf",  # or openvino/tensorrt
        "h5_path": os.path.join(base_dir, "./model.h5"),
        "script_path": os.path.join(base_dir, "model.py"),
        "checkpoint_path": "",
        "frozen_graph_path": "",
        "export_path": os.path.join(base_dir, "serving_model"),
        "input_layer_names": [
            "conv2d_1"
        ],
        "output_layer_names": [
            "dense_2"
        ],
        "input_names": [
            "conv2d_1_input:0"
        ],
        "input_formats": [
            "channels_last"
        ],
        "output_names": [
            "dense_2/Softmax:0"
        ],
        "input_signatures": [
            "image"
        ],
        "output_signatures": [
            "label"
        ],
        "model_name": "test",
        "job_id": "mnist",
        "callback": "",
        "max_batch_size": 128
    }

    _LOGGER.info('Prepare run test, request: %s', request)
    compiler = create_compiler(create_config(from_source='json', json_message=json.dumps(request)))
    result = compiler.compile()
    _LOGGER.info('result: %s', result)


if __name__ == "__main__":
    _main()
