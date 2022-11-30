# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a tool which compile model to serving model.
The request of compiling model must match config_schema.json
"""

import json
import argparse

import model_compiler


def _main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--request', type=str)
    args = parser.parse_args()

    request = json.loads(args.request)
    result = model_compiler.compile_model(request)
    print(result)


if __name__ == "__main__":
    _main()
