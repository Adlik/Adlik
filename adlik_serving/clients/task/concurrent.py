# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a help script to executing task concurrently
"""

import argparse
import multiprocessing
import os
import subprocess
import sys
import time

FLAGS = None


class MyProcess(multiprocessing.Process):
    def __init__(self, index):
        super(MyProcess, self).__init__()
        self.num = index

    def run(self):
        script_path = os.path.join(os.path.dirname(sys.argv[0]), "task_client.py")
        start = time.time()
        subprocess.check_call(['python3', script_path])
        end = time.time()
        print('Run process {} use {} s'.format(self.num, end - start))


def _main():
    processes = [MyProcess(i) for i in range(FLAGS.concurrent_num)]
    for p in processes:
        p.start()

    for p in processes:
        p.join()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--concurrent-num', type=int, required=False, default=2,
                        help='Number of concurrent task')
    FLAGS = parser.parse_args()
    _main()
