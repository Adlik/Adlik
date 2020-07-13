# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
The CMD script
"""

import subprocess
import argparse
import os
import time
from grpc._channel import _InactiveRpcError
from adlik_serving import PredictContext  # pylint: disable=import-error


def _test_connect_of_serving(args):
    try:
        PredictContext(os.getenv('MODEL_NAME'), url=args.url, protocol=args.protocol, verbose=True)
        return True
    except _InactiveRpcError:
        return False


def _main(args):
    compile_command = ['sh', '-c', args.compile_script]
    serving_command = ['sh', '-c', args.serving_script]
    client_command = ['sh', '-c', args.client_script]
    subprocess.run(compile_command)
    with subprocess.Popen(serving_command) as process:
        while not _test_connect_of_serving(args):
            time.sleep(1)
        subprocess.run(client_command)
        process.kill()


if __name__ == '__main__':
    ARGS_PARSER = argparse.ArgumentParser()
    ARGS_PARSER.add_argument('-s', '--serving-script', type=str, required=True, help='The serving script')
    ARGS_PARSER.add_argument('-c', '--client-script', type=str, required=True, help='The client script')
    ARGS_PARSER.add_argument('-cs', '--compile-script', type=str, required=True, help='The compile script')
    ARGS_PARSER.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                             help='Server URL. Default is localhost:8500.')
    ARGS_PARSER.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                             help='Protocol ("http"/"grpc") used to ' + 'communicate with service. Default is "grpc".')
    PARSE_ARGS = ARGS_PARSER.parse_args()
    _main(PARSE_ARGS)