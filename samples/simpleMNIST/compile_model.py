# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a mnist sample which compile keras h5 model to tf serving/openvino/tensorrt model
"""
import os

from model_compiler.config import create_config_from_obj
from model_compiler.runtime import create_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "tf",  # or openvino, tensorrt
        "h5_path": os.path.join(base_dir, "./model/mnist.h5"),
        "script_path": os.path.join(base_dir, "train_model.py"),
        "export_path": os.path.join(base_dir, "model_repos"),
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
        "model_name": "mnist",
        "job_id": "mnist",
        "max_batch_size": 128
    }

    compiler = create_compiler(create_config_from_obj(request))
    result = compiler.compile()
    print(result)


if __name__ == "__main__":
    _main()
