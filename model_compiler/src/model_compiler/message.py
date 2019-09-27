"""
Some utils about message
"""
import time

import requests

from .log_util import get_logger

_LOGGER = get_logger(__name__)


def fail(error_msg):
    """
    Create failure response
    :param error_msg:
    :return:
    """
    return {'status': 'failure', 'error_msg': error_msg}


def success(path):
    """
    Create success response
    :param path:
    :return:
    """
    return {'status': 'success', 'path': path}


def is_success(msg):
    """
    Whether message is success
    :param msg:
    :return:
    """
    return msg['status' == 'success']


def send_response(url, message):
    """
    Send compile result to url
    :param url:
    :param message:
    :return:
    """
    retries = 0
    max_retries = 3

    while retries <= max_retries:
        try:
            result = requests.put(url, json=message)
            _LOGGER.info('Send response to url: %s, message: %s, response: code: %s, content: %s', url, message,
                         result.status_code, result.content)
            return result
        except Exception as exception:  # pylint:disable=broad-except
            _LOGGER.error('send_response: error occur %s, url %s', repr(exception), url)
            time.sleep(2)
            retries += 1
            continue
