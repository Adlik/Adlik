"""
Compress file or directory
"""

import os
import zipfile


def compress_dir(source_dir, zip_file_path):
    """
    Compress a directory into .zip file
    :param source_dir: path of directory to be compressed
    :param zip_file_path: path of zip file
    :return:
    """
    zip_file = zipfile.ZipFile(zip_file_path, "w", zipfile.ZIP_DEFLATED)
    for path, _, filenames in os.walk(source_dir):
        fpath = path.replace(source_dir, '')
        for filename in filenames:
            zip_file.write(os.path.join(path, filename), os.path.join(fpath, filename))
    zip_file.close()
    return zip_file_path
