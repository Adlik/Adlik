# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a recommend sample which compile pytorch onnx model to openvino model.
The request of compiling model must match config_schema.json
"""
import os
import model_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "openvino",
        "input_model": os.path.join(base_dir, "model/ENMF.onnx"),
        "export_path": os.path.join(base_dir, "model_repos"),
        "input_names": [
            "input"
        ],
        "input_formats": [
            "channels_first"
        ],
        "output_names": [
            "output"
        ],
        "model_name": "ENMF",
        "job_id": "ENMF_pytorch",
        "callback": "",
        "max_batch_size": 2048,
        "input_shapes": [
            []
        ]
    }

    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
