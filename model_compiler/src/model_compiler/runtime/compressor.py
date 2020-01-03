# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Compress file or directory
"""

import os
import zipfile


def compress_dir(source_list, zip_file_path):
    """
    Compress a source list into .zip file
    :param source_list: source path list to be compressed
    :param zip_file_path: path of zip file
    :return:
    """
    zip_file = zipfile.ZipFile(zip_file_path, "w", zipfile.ZIP_DEFLATED)
    for source in source_list:
        basename = os.path.basename(source)
        zip_file.write(source, basename)
        if os.path.isdir(source):
            for path, _, filenames in os.walk(source):
                fpath = path.replace(source, basename)
                for filename in filenames:
                    zip_file.write(os.path.join(path, filename), os.path.join(fpath, filename))
    zip_file.close()
    return zip_file_path
