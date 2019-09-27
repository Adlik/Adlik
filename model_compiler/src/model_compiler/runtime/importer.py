"""
import file util
"""

import os
import sys


def import_file(file_path):
    """
    import a python file as module
    :param file_path:
    :return: module
    """
    absolute_filename = os.path.abspath(file_path)
    absolute_path_with_module, _ = os.path.splitext(absolute_filename)
    _, module_name = os.path.split(absolute_path_with_module)

    if module_name in sys.modules:
        del sys.modules[module_name]

    try:
        from importlib import util
        spec = util.spec_from_file_location(module_name, absolute_filename)
        keras_module = util.module_from_spec(spec)
        spec.loader.exec_module(keras_module)
        return keras_module
    except ImportError:
        import imp
        return imp.load_source(module_name, absolute_filename)
