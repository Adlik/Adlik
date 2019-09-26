#!/usr/bin/env python3

import re
import subprocess


def main():
    commit_message_subject = subprocess.check_output(args=['git', 'log', '-1', '--format=%s'], universal_newlines=True)

    if not re.fullmatch(r'[A-Z0-9][^\s]*( [^\s]+)*\n', commit_message_subject):
        print('Invalid commit message subject:')
        print(commit_message_subject)

        exit(1)


if __name__ == "__main__":
    main()
