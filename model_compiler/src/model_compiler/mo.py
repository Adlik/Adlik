# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Entry for model compiler.
"""

from .config import create_config_from_obj
from .runtime import create_compiler


def compile_model(request):
    """
    Compile serving model
    :param request: dict, must match config_schema.json
    :return:
    """
    compiler = create_compiler(create_config_from_obj(request))
    return compiler.compile()
