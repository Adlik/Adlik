# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a resnet50 sample which compile paddle pdparams model to paddle serving model.
The request of compiling model must match config_schema.json
"""

import os
import model_compiler


def _main():
    base_dir = os.path.dirname(__file__)
    request = {
        "serving_type": "paddle",
        "input_model": os.path.join(base_dir, "model_resnet50"),
        "export_path": os.path.join(base_dir, "model_repos_resnet50"),
        "input_formats": [
            "channels_first"
        ],
        "input_names": [
            "image"
        ],
        "output_names": [
            "linear_1.tmp_1",
            "pool2d_4.tmp_0"
        ],
        'model_filename': 'resnet50.pdmodel',
        'params_filename': 'resnet50.pdiparams',
        'model_name': 'resnet50',
        'max_batch_size': 128
    }

    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
