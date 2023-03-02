# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a topsinference sample which compile onnx model to topsinfrence model.
"""
import os

import model_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "engine",  # or openvino, tensorrt
        "input_model": os.path.join(base_dir, "./model/resnet50.onnx"),
        "export_path": os.path.join(base_dir, "model_repos"),
        "input_layer_names": [
            "input.19"
        ],
        "output_layer_names": [
            "442"
        ],
        "input_formats": [
            "channels_first"
        ],
        "input_signatures": [
            "image"
        ],
        "output_signatures": [
            "label"
        ],
        "model_name": "resnet50",
        "max_batch_size": 128
    }

    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
