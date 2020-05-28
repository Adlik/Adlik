#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import csv
import os
import shutil
import subprocess
import tarfile
import time
import zipfile
from argparse import ArgumentParser
from os import path
from tarfile import TarInfo
from tempfile import TemporaryDirectory
from urllib import request
from zipfile import ZipFile, ZipInfo


def _get_arguments():
    arg_parser = ArgumentParser()

    arg_parser.add_argument('source_url')
    arg_parser.add_argument('target_dir')

    return arg_parser.parse_args()


def _download_file(url, target):
    with request.urlopen(url) as response, open(target, 'wb') as target_file:
        shutil.copyfileobj(response, target_file)


def _extract_ar(source_ar, file_name, target_file):
    with open(target_file, 'wb') as f:
        subprocess.run(['ar', 'p', source_ar, file_name], stdout=f, check=True)


def _extract_name_and_version(metadata_file):
    name_key = 'Name: '
    version_key = 'Version: '
    package_name = None
    package_version = None

    for line in metadata_file.read().decode().splitlines():
        if line.startswith(name_key):
            package_name = line[len(name_key):]
        elif line.startswith(version_key):
            package_version = line[len(version_key):]

    if package_name is None or package_version is None:
        raise ValueError

    return package_name, package_version


def _extract_tag(metadata_file):
    tag_key = 'Tag: '

    for line in metadata_file.read().decode().splitlines():
        if line.startswith(tag_key):
            return line[len(tag_key):]

    raise ValueError


def _tar_info_to_zip_info(tar_info: TarInfo, file_name) -> ZipInfo:
    modified_time = time.gmtime(tar_info.mtime)

    zip_info = ZipInfo(filename=file_name,
                       date_time=(modified_time.tm_year,
                                  modified_time.tm_mon,
                                  modified_time.tm_mday,
                                  modified_time.tm_hour,
                                  modified_time.tm_min,
                                  modified_time.tm_sec))

    if tar_info.isfile():
        zip_info.external_attr = tar_info.mode << 16
    else:
        raise ValueError

    return zip_info


def main(args):
    with TemporaryDirectory() as workspace:
        debian_package = path.join(workspace, 'debian-package.deb')
        debian_package_data = path.join(workspace, 'debian-package-data.tar.xz')

        _download_file(args.source_url, debian_package)
        _extract_ar(debian_package, 'data.tar.xz', debian_package_data)

        with open(debian_package_data, 'rb') as debian_package_data_file, \
                tarfile.open(mode='r:xz', fileobj=debian_package_data_file) as tar_file:
            for record_tar_info in filter(lambda ti: ti.name.endswith('.dist-info/RECORD'), tar_file):
                dist_info_dir = path.dirname(record_tar_info.name)
                packages_dir = path.dirname(dist_info_dir)

                with tar_file.extractfile(path.join(dist_info_dir, 'METADATA')) as metadata_file:
                    package_name, package_version = _extract_name_and_version(metadata_file)

                with tar_file.extractfile(path.join(dist_info_dir, 'WHEEL')) as wheel_file:
                    tag = _extract_tag(wheel_file)

                os.makedirs(args.target_dir, exist_ok=True)

                with ZipFile(file=path.join(args.target_dir, f'{package_name}-{package_version}-{tag}.whl'),
                             mode='w') as zip_file, \
                        tar_file.extractfile(record_tar_info) as record_file:
                    for record in csv.reader(record_file.read().decode().splitlines()):
                        file_name = record[0]
                        tar_info = tar_file.getmember(path.join(packages_dir, file_name))
                        zip_info = _tar_info_to_zip_info(tar_info, file_name)

                        zip_file.writestr(zinfo_or_arcname=zip_info,
                                          data=tar_file.extractfile(tar_info).read(),
                                          compress_type=zipfile.ZIP_DEFLATED)


if __name__ == '__main__':
    main(_get_arguments())
