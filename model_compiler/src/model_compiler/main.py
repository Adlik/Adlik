# -*- coding: utf-8 -*-

"""
Entry for model compiler.
"""

from .config import create_config
from .log_util import get_logger, setup_logger
from .message import fail, send_response
from .runtime import create_compiler

_LOGGER = get_logger(__name__)


def main():
    """
    Entry of module
    :return:
    """
    setup_logger()

    config = None
    try:
        config = create_config()
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
    main()
