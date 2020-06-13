# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Compile the model.
"""

import os
import json
import argparse
import model_compiler  # pylint:disable=import-error


def _get_request(request_file, test_model_dir):
    request = json.load(request_file)
    model_dir = request["input_model"]
    request["input_model"] = os.path.join(test_model_dir, model_dir)
    export_dir = request["export_path"]
    request["export_path"] = os.path.join(test_model_dir, export_dir)
    return request


def compile_model(args):
    """
    Compile the model.
    """

    request_dir = os.path.join(args.test_model_path, args.serving_model_json)
    try:
        with open(request_dir, 'r') as request_file:
            test_model_dir = args.test_model_path
            request = _get_request(request_file, test_model_dir)
            result = model_compiler.compile_model(request)
            print(result)
    except FileNotFoundError:
        print(f"Can not compile the model in {os.path.join(test_model_dir, args.model_path)}")


if __name__ == '__main__':
    ARGS_PARSER = argparse.ArgumentParser()
    ARGS_PARSER.add_argument('-t', '--test-model-path', type=str, required=True, help='The path of test model')
    ARGS_PARSER.add_argument('-s', '--serving-model-json', type=str, default='serving_model.json',
                             help='The json of model')
    PARSE_ARGS = ARGS_PARSER.parse_args()
    compile_model(PARSE_ARGS)
