# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import argparse
import json
import os

from . import compiler
from . import utilities


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-s', '--config-source',
                        default='env',
                        choices=['env', 'json'],
                        required=False,
                        help='source of model config, can be env or json')

    parser.add_argument('-p', '--json-file', type=str, required=False, default=None, help='path of json file')

    args = parser.parse_args()
    env = os.environ.copy()

    try:
        if args.config_source == 'json':
            with open(args.json_file) as json_file:
                result = compiler.compile_from_json(json.load(json_file))
        else:
            result = compiler.compile_from_env()
        if env.get('CALLBACK'):
            utilities.send_response(env['CALLBACK'], {"status": "success", "path": result})
    except Exception as error:  # pylint:disable=broad-except
        if env.get('CALLBACK'):
            utilities.send_response(env['CALLBACK'], {'status': 'failure', 'error_msg': str(error)})


if __name__ == '__main__':
    main()
