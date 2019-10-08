#!/usr/bin/env python3

import re
import subprocess


def main():
    subject_regex = re.compile(r'[A-Z0-9][^\s]*( [^\s]+)*')
    has_failure = False

    commit_messages = subprocess.check_output(args=['git', 'log', '--format=%s', 'origin/master..'],
                                              universal_newlines=True)

    for subject in commit_messages.splitlines():
        if subject_regex.fullmatch(subject):
            print(f'Valid commit message subject: {subject}')
        else:
            has_failure = True
            print(f'Invalid commit message subject: {subject}')

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main()
