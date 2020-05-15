from . import compiler


def compile_model(config):
    try:
        return {
            'status': 'success',
            'path': compiler.compile_from_json(config)
        }
    except Exception as error:  # pylint: disable=broad-except
        return {
            'status': 'failure',
            'error_msg': str(error)
        }
