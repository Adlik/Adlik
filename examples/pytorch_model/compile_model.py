# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a mnist sample which compile keras h5 model to tf serving/openvino/tensorrt model.
The request of compiling model must match config_schema.json
"""
import os

import model_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "openvino",
        "input_model": os.path.join(base_dir, "./model/mnist.onnx"),
        "export_path": os.path.join(base_dir, "model_repos"),
        "input_names": [
            "input.1"
        ],
        "input_formats": [
            "channels_first"
        ],
        "output_names": [
            "20"
        ],
        "input_signatures": [
            "image"
        ],
        "output_signatures": [
            "label"
        ],
        "model_name": "mnist",
        "job_id": "mnist_pytorch",
        "callback": "",
        "max_batch_size": 128
    }

    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
