"""
This is a sample for Adlik serving prediction,
use model plan_float16_float32_float32 which runs on tensorRT platform
"""

import argparse
import base64
import sys

import numpy as np
import requests
import six

FLAGS = None

_DTYPE_NPTYPE_MAPS = {'DT_FLOAT': np.float32,
                      'DT_DOUBLE': np.float64,
                      'DT_INT32': np.int32,
                      'DT_UINT8': np.uint8,
                      'DT_INT16': np.int16,
                      'DT_INT8': np.int8,
                      'DT_INT64': np.int64,
                      'DT_BOOL': np.bool,
                      'DT_UINT16': np.uint16,
                      'DT_HALF': np.float16}

_NPTYPE_DTYPE_MAPS = {v: k for k, v in _DTYPE_NPTYPE_MAPS.items()}


def tensor_dtype_to_np_dtype(tensor_dtype):
    """
    Convert tensor dtype to numpy dtype
    :param tensor_dtype:
    :return: numpy dtype
    """
    return _DTYPE_NPTYPE_MAPS.get(tensor_dtype, None)


def np_dtype_to_tensor_dtype(np_dtype):
    """
    Convert numpy dtype to tensor dtype
    :param np_dtype:
    :return: tensor dtype
    """
    for key, val in six.iteritems(_NPTYPE_DTYPE_MAPS):
        if key == np_dtype:
            return val
    return None


def _make_request(inputs_dict, output_names, batch_size, signature=None):
    request = {'batch_size': batch_size}
    if signature is not None:
        request['model_spec'] = {'signature': signature}

    request['inputs'] = {}
    for name, value in inputs_dict.items():
        request['inputs'][name] = {'dtype': np_dtype_to_tensor_dtype(value.dtype),
                                   'tensor_shape': {'dim': [{'size': i} for i in value.shape]},
                                   'tensor_content': base64.b64encode(value.tobytes()).decode('utf-8')}

    request["output_filter"] = {i: {} for i in output_names}

    return request


def _get_outputs(response, output_names):
    result = {}
    for name in output_names:
        output = response['outputs'][name]['tensor']
        shape = [int(i['size']) for i in output['tensor_shape']['dim']]
        result[name] = np.frombuffer(base64.b64decode(output['tensor_content']),
                                     dtype=tensor_dtype_to_np_dtype(output['dtype'])).reshape(shape)

    return result


def _main():
    model_name = "plan_float16_float32_float32"
    model_version = 1
    batch_size = 1

    input0_data = np.arange(start=0, stop=16, dtype=np.float16)
    input1_data = np.ones(shape=16, dtype=np.float16)
    output_names = ['OUTPUT0', 'OUTPUT1']

    server_url = 'http://%s/v1/models/%s/versions/%s:predict' % (FLAGS.url, model_name, model_version)

    predict_request = _make_request({'INPUT0': input0_data.reshape((1, 16)), 'INPUT1': input1_data.reshape((1, 16))},
                                    output_names, batch_size)

    response = requests.post(server_url, json=predict_request)
    response.raise_for_status()

    result = _get_outputs(response.json(), output_names)
    output0_data = result['OUTPUT0'][0]
    output1_data = result['OUTPUT1'][0]

    for i in range(16):
        print(str(input0_data[i]) + " + " + str(input1_data[i]) + " = " + str(output0_data[i]))
        print(str(input0_data[i]) + " - " + str(input1_data[i]) + " = " + str(output1_data[i]))
        if (input0_data[i] + input1_data[i]) != output0_data[i]:
            print("error: incorrect sum")
            sys.exit(1)
        if (input0_data[i] - input1_data[i]) != output1_data[i]:
            print("error: incorrect difference")
            sys.exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8501',
                        help='Adlik serving server URL. Default is localhost:8501.')

    FLAGS = parser.parse_args()
    _main()
