# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
The CMD script
"""

import subprocess
import argparse


def _main(args):
    compile_command = ['sh', '-c', args.compile_script]
    serving_command = ['sh', '-c', args.serving_script]
    client_command = ['sh', '-c', args.client_script]
    subprocess.run(compile_command)
    with subprocess.Popen(serving_command) as process:
        subprocess.run(client_command)
        process.kill()


if __name__ == '__main__':
    ARGS_PARSER = argparse.ArgumentParser()
    ARGS_PARSER.add_argument('-s', '--serving-script', type=str, required=True,
                             help='The serving script')
    ARGS_PARSER.add_argument('-c', '--client-script', type=str, required=True,
                             help='The client script')
    ARGS_PARSER.add_argument('-cs', '--compile-script', type=str, required=True,
                             help='The compile script')
    PARSE_ARGS = ARGS_PARSER.parse_args()
    _main(PARSE_ARGS)
