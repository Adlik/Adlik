# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import argparse
import json

from . import compiler


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-s', '--config-source',
                        default='env',
                        choices=['env', 'json'],
                        required=False,
                        help='source of model config, can be env or json')

    parser.add_argument('-p', '--json-file', type=str, required=False, default=None, help='path of json file')

    args = parser.parse_args()

    if args.config_source == 'json':
        with open(args.json_file) as json_file:
            compiler.compile_from_json(json.load(json_file))
    else:
        compiler.compile_from_env()


if __name__ == '__main__':
    main()
