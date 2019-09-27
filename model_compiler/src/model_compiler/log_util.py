# -*- coding: utf-8 -*-

"""
Log tool
Before using Logger, you need to call setup to load config at least one time
If parent process call setup, child process will hold the same config,
if you want to change config in child process, you need to re-call setup
"""

import logging.config


def setup_logger(log_path=None):
    """
    Method for setting up logger
    :param log_path:
    :return:
    """

    config = get_default_config()

    if log_path:
        config['handlers']['file']['filename'] = log_path
        config['loggers']['']['handlers'].append('file')
    else:
        config['handlers'].pop('file')
    logging.config.dictConfig(config)

    tf_logger = logging.getLogger('tensorflow')
    tf_logger.setLevel(logging.WARNING)


def get_default_config():
    """
    Get a default config of logging
    :return:
    """

    return {
        'version': 1,
        'disable_existing_loggers': False,
        'formatters': {
            'standard': {
                'format': '[%(asctime)s %(name)s:%(lineno)d %(levelname)s] %(message)s'
            }
        },
        'handlers': {
            'console': {
                'formatter': 'standard',
                'class': 'logging.StreamHandler',
                'stream': 'ext://sys.stdout'
            },
            'file': {
                'formatter': 'standard',
                'class': 'logging.handlers.RotatingFileHandler',
                'filename': ''
            }
        },
        'loggers': {
            '': {
                'handlers': ['console'],
                'level': 'NOTSET',
            }
        }
    }


def get_logger(name):
    """
    Get or create a logger with the specified name.

    Recommended usage: get_logger(__name__)
    """

    return logging.getLogger(name)
