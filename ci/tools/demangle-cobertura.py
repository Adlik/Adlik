#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import argparse
import subprocess
from xml.etree import ElementTree


def _get_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('source')
    parser.add_argument('target')

    return parser.parse_args()


def _load_xml(path: str) -> ElementTree:
    return ElementTree.parse(path)


def _demangle(name: str) -> str:
    return subprocess.check_output(args=['c++filt', name], universal_newlines=True).strip()


def _do_demangle(document: ElementTree):
    for package in document.getroot().find('packages'):
        for class_ in package.find('classes'):
            for method in class_.find('methods'):
                attrib = method.attrib

                attrib['name'] = _demangle(attrib['name'])


def _save_xml(document: ElementTree, path: str):
    with open(path, 'wb') as file:
        file.write(b"""<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE coverage SYSTEM "http://cobertura.sourceforge.net/xml/coverage-04.dtd">
""")

        document.write(file, encoding='utf-8')


def main(args):
    document = _load_xml(args.source)

    _do_demangle(document)

    _save_xml(document, args.target)


if __name__ == '__main__':
    main(_get_args())
