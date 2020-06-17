# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
The test of automatic test.
"""

import unittest
import subprocess
import os


class TestAutomaticTest(unittest.TestCase):
    """
    The test of automatic test
    """

    @staticmethod
    def test_automatic_test():
        """
        The test of automatic test.
        """

        base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
        model_command = ['python3', 'benchmark/tests/test_model/mnist_keras/mnist_keras.py']
        subprocess.run(args=model_command, cwd=base_dir, check=True)
        command = ['python3', 'benchmark/src/automatic_test.py',
                   '-d', 'benchmark/tests/docker_test/openvino.Dockerfile',
                   '-s', 'openvino',
                   '-b', '.',
                   '-a', '.',
                   '-m', 'mnist',
                   '-c', 'benchmark/tests/client_script/client_script.sh',
                   '-ss', 'benchmark/tests/serving_script/openvino_serving_script.sh',
                   '-l', os.path.join(base_dir, 'benchmark/log'),
                   '-tm', 'benchmark/tests/test_model/mnist_keras',
                   '-cis', 'mnist_client.py',
                   '-i', 'mnist.png',
                   '-cs', 'benchmark/tests/compile_script/openvino_compile_script.sh']
        with subprocess.Popen(args=command, cwd=base_dir) as process:
            print(process.stdout)
