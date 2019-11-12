# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Entry for model compiler.
"""
import argparse

from .config import create_config
from .log_util import get_logger, setup_logger
from .message import fail, send_response
from .runtime import create_compiler

_LOGGER = get_logger(__name__)
FLAGS = None


def main():
    """
    Entry of module
    :return:
    """
    setup_logger()

    config = None
    try:
        config = create_config(FLAGS.config_source, FLAGS.json_file)
        _LOGGER.info('Begin to compile model, config: %s', config)
    except Exception as error:  # pylint:disable=broad-except
        _LOGGER.fatal("create config fail, error: %s, and can't send response", error)
        exit(1)

    try:
        compiler = create_compiler(config)
        _LOGGER.info('Create compiler: %s', compiler)
        result = compiler.compile()
        _LOGGER.info('Compile result: %s', result)
        send_response(config.get_attribute('callback'), result)
    except Exception as error:  # pylint:disable=broad-except
        _LOGGER.error('Model compiler failed, error: %s', error)
        _LOGGER.exception(error)
        send_response(config.get_attribute('callback'), fail(str(error)))


if __name__ == "__main__":
    PARSER = argparse.ArgumentParser()
    PARSER.add_argument('-s', '--config-source', type=str, required=False, default='env',
                        help='source of model config, can be env or json')
    PARSER.add_argument('-p', '--json-file', type=str, required=False, default=None,
                        help='path of json file')

    FLAGS = PARSER.parse_args()
    main()
