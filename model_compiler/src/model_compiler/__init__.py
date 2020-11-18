# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import traceback

from . import compiler


def compile_model(config):
    try:
        return {
            'status': 'success',
            'path': compiler.compile_from_json(config)
        }
    except Exception as error:  # pylint: disable=broad-except
        traceback.print_exc()

        return {
            'status': 'failure',
            'error_msg': str(error)
        }
