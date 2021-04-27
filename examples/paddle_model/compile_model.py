# Copyright 2021 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a ResNet50 sample which compile PaddlePaddle model(ONNX format) to tf serving/openvino/tensorrt model.
The request of compiling model must match config_schema.json
"""
import os

import model_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "openvino",
        "input_model": os.path.join(base_dir, "./model/resnet50.onnx"),
        "export_path": os.path.join(base_dir, "model_repos"),
        "input_names": [
            "image"
        ],
        "input_formats": [
            "channels_first"
        ],
        "output_names": [
            "save_infer_model/scale_0.tmp_0"
        ],
        "input_signatures": [
            "image"
        ],
        "output_signatures": [
            "label"
        ],
        "model_name": "ResNet50_vd_ssld",
        "job_id": "ResNet50_PaddlePaddle",
        "callback": "",
        "max_batch_size": 128
    }

    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
