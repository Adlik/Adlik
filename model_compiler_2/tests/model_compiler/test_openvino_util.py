import contextlib
import os
from os import path
from tempfile import TemporaryDirectory
from unittest import TestCase

from model_compiler import openvino_util


def _set_env(env, key, value):
    if value is None:
        try:
            del env[key]
        except KeyError:
            pass
    else:
        env[key] = value


@contextlib.contextmanager
def _use_cvsdk_dir(dir_path):
    env = os.environ
    key = 'INTEL_CVSDK_DIR'
    saved = os.getenv(key)

    _set_env(env, key, dir_path)

    try:
        yield
    finally:
        _set_env(env, key, saved)


class OpenVinoUtilTestCase(TestCase):
    def test_set_env(self):
        env = {'foo': 'bar'}

        _set_env(env=env, key='def', value=None)

        self.assertEqual(env, {'foo': 'bar'})

        _set_env(env=env, key='def', value='ghi')

        self.assertEqual(env, {'foo': 'bar', 'def': 'ghi'})

        _set_env(env=env, key='foo', value=None)

        self.assertEqual(env, {'def': 'ghi'})

    def test_get_version_no_file(self):
        with TemporaryDirectory() as cvsdk_dir, _use_cvsdk_dir(cvsdk_dir):
            result = openvino_util.get_version()

        self.assertEqual(result, 'unknown version')

    def test_get_version_custom_version(self):
        with TemporaryDirectory() as cvsdk_dir:
            model_optimizer_dir = path.join(cvsdk_dir, 'deployment_tools', 'model_optimizer')

            os.makedirs(model_optimizer_dir)

            with open(path.join(model_optimizer_dir, 'version.txt'), 'w') as version_file:
                version_file.writelines('foo bar')

            with _use_cvsdk_dir(cvsdk_dir):
                result = openvino_util.get_version()

        self.assertEqual(result, 'foo bar')
