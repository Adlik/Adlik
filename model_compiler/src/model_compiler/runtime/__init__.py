"""
Model compiler runtime
"""


def create_compiler(config):
    """
    Get model compiler runtime
    :param config: Config object
    :return: class of Compiler
    """
    serving_type = config.get_attribute('serving_type')
    if serving_type not in ['tf', 'tensorrt', 'openvino']:
        raise Exception('Not support serving type %s' % serving_type)
    if serving_type == 'tf':
        from .tf.compiler import Compiler
        return Compiler(config)
    elif serving_type == 'tensorrt':
        from .tensorrt.compiler import Compiler
        return Compiler(config)
    elif serving_type == 'openvino':
        from .openvino.compiler import Compiler
        return Compiler(config)
    else:
        raise Exception('Not support platform {}'.format(serving_type))
