#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import re
import subprocess


def main():
    subject_regex = re.compile(r'[^a-z\s]\S*( \S+)*')
    has_failure = False

    commit_messages = subprocess.check_output(args=['git', 'log', '--format=%s', 'origin/master..'],
                                              universal_newlines=True)

    for subject in commit_messages.splitlines():
        if subject_regex.fullmatch(subject) and not subject.endswith('.'):
            print('Valid commit message subject:', subject)
        else:
            has_failure = True
            print('Invalid commit message subject:', subject)

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main()
