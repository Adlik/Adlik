"""
Context for adlik serving predict service
"""

# pylint: disable=unused-import

import time

from google.protobuf import json_format
import grpc
import numpy as np
import requests
from adlik_serving.apis import get_model_meta_pb2, predict_pb2, predict_service_pb2_grpc
from adlik_serving.framework.domain import model_config_pb2, model_spec_pb2
import tensorflow as tf
from tensorflow.core.framework import tensor_pb2, types_pb2

model_config_pb2 = model_config_pb2


def tensor_dtype_to_np_dtype(tensor_dtype):
    """
    Convert tensor dtype to numpy dtype
    :param tensor_dtype:
    :return: numpy dtype
    """
    types_map = {types_pb2.DT_FLOAT: np.float32,
                 types_pb2.DT_DOUBLE: np.float64,
                 types_pb2.DT_INT32: np.int32,
                 types_pb2.DT_UINT8: np.uint8,
                 types_pb2.DT_INT16: np.int16,
                 types_pb2.DT_INT8: np.int8,
                 types_pb2.DT_INT64: np.int64,
                 types_pb2.DT_BOOL: np.bool,
                 types_pb2.DT_UINT16: np.uint16,
                 types_pb2.DT_HALF: np.float16}
    return types_map.get(tensor_dtype, None)


class _Output(object):
    """
    Output Definition in Predict response where 'tensor' field is numpy.ndarray but not tensor proto
    """

    def __init__(self, output_proto):
        self.tensor = tf.make_ndarray(output_proto.tensor)
        self.batch_classes = [[{'idx': cls.idx, 'score': cls.score, 'label': cls.label} for cls in
                               batch.cls] for batch in output_proto.batch_classes]


class _HttpRunner(object):
    def __init__(self, model_name, model_version, signature, url, verbose=False):
        self._url = '%s/v1/models/%s' % (url, model_name)
        if not self._url.startswith('http://'):
            self._url = ''.join(['http://', self._url])
        if model_version is not None:
            self._url = self._url + '/versions/%s' % model_version
        self._signature = signature
        self._verbose = verbose

    def predict(self, request):
        response = requests.post(self._url + ":predict",
                                 data=json_format.MessageToJson(request, preserving_proto_field_name=True))
        response.raise_for_status()
        return json_format.Parse(response.content, predict_pb2.PredictResponse())

    def get_model_meta(self, _):
        response = requests.get(self._url + '/metadata')
        response.raise_for_status()
        response = json_format.Parse(response.content, get_model_meta_pb2.GetModelMetaResponse())
        return response.config


class _GrpcRunner(object):
    """
    An GrpcPredictContext object is used to run predict on grpc protocol for a specific model.
    """

    def __init__(self, model_name, model_version, signature, url, grpc_domain=None, credentials=None, verbose=False):
        self._model_name = model_name
        self._model_version = model_version
        self._signature = signature
        self._verbose = verbose
        if credentials is None:
            channel = grpc.insecure_channel(url)
        else:
            credentials = grpc.ssl_channel_credentials(credentials)
            channel = grpc.secure_channel(url,
                                          credentials=credentials,
                                          options=[("grpc.ssl_target_name_override", grpc_domain)])
        self._stub = predict_service_pb2_grpc.PredictServiceStub(channel)

    def predict(self, request):
        response = self._stub.predict(request)
        return response

    def async_predict(self, request):
        """
        return request
        """
        return self._stub.predict.future(request)

    def get_model_meta(self, model_spec):
        request = get_model_meta_pb2.GetModelMetaRequest()
        request.model_spec.CopyFrom(model_spec)
        response = self._stub.getModelMeta(request)
        assert (response.model_spec.name == self._model_name)
        return response.config


def _create_runner(model_name, model_version, signature, url, protocol, grpc_domain=None,
                   credentials=None, verbose=False):
    if protocol == 'http':
        return _HttpRunner(model_name, model_version, signature, url, verbose=verbose)
    else:
        return _GrpcRunner(model_name, model_version, signature, url, grpc_domain=grpc_domain,
                           credentials=credentials, verbose=verbose)


class PredictContext(object):
    """
    An PredictContext object is used to run predict on server for a specific model.

    Once created an InferContext object can be used repeatedly to
    perform inference using the model.

    Parameters
    ----------
    model_name : str
        The name of the model to get status or do prediction.
    model_version : int
        The version of the model to use for inference.
        Default is None which indicate that the latest (i.e. highest version number)
        version should be used.
    signature : str
        The signature of the model.
    url : str
        The inference server URL, e.g. localhost:8500.
    protocol : ProtocolType
        The protocol used to communicate with the server, like grpc or http.
    verbose : bool
        If True generate verbose output.
    """

    def __init__(self, model_name, model_version=None, signature='predict', url='localhost:8500', protocol='grpc',
                 grpc_domain=None, credentials=None, verbose=False):
        self._model_name = model_name
        self._model_version = model_version
        self._signature = signature
        self._model_spec = self._make_model_spec()
        self._verbose = verbose
        if protocol not in ['grpc', 'http']:
            raise "Only support grpc or http protocol, don't support {}.".format(protocol)
        self._runner = _create_runner(model_name, model_version, signature, url, protocol, grpc_domain=grpc_domain,
                                      credentials=credentials, verbose=verbose)
        self._model_config = self._runner.get_model_meta(self._model_spec)

    @property
    def model_config(self):
        """
        Get model meta information, should send get meta request to server and parse meta from response
        :return:
        """
        return self._model_config

    def run(self, inputs, outputs, batch_size=1, iteration=1):
        """
        Run predict using the supplied 'inputs' to calculate the outputs specified by 'outputs'.
        :param inputs: Dictionary from input name to the value(s) for that input.
            An input value is specified as a numpy array. Each input in the dictionary
            maps to a list of values (i.e. a list of numpy array objects), where the length
            of the list must equal the 'batch_size'.
        :param outputs: list of string or dictionary of name: type indicating the output names should be returned
        :param batch_size: The batch size of the prediction. Each input must provide
            an appropriately sized batch of inputs.
        :param iteration: iteration times for send request, just for evaluating time of server process request
        :return: A dictionary from output name to the list of values for
            that output (one list element for each entry of the
            batch).
        """
        request = self._prepare_request(inputs, outputs, batch_size)
        response = None
        start = time.time()
        for i in range(iteration):
            response = self._runner.predict(request)
        end = time.time()
        if self._verbose:
            print('Run model {} predict (batch size={}) use {} ms'.format(
                self._model_name, batch_size, (end - start) * 1000 / iteration))
        result = self.process_response(outputs, response)
        return result

    def async_run(self, inputs, outputs, batch_size=1, iteration=1):
        request = self._prepare_request(inputs, outputs, batch_size)
        return self._runner.async_predict(request)

    def _prepare_request(self, inputs, outputs, batch_size):
        request = predict_pb2.PredictRequest()
        request.model_spec.CopyFrom(self._model_spec)
        request.batch_size = batch_size
        self._prepare_inputs(request, inputs, batch_size)
        self._prepare_outputs(request, outputs)
        return request

    @staticmethod
    def _prepare_inputs(request, inputs, batch_size):
        for name, value in inputs.items():
            if isinstance(value, tensor_pb2.TensorProto):
                pass
            elif isinstance(value, (list, tuple)):
                if len(value) != batch_size:
                    raise 'Number of input {} must equal to batch size'.format(name)
                value = np.array(value)
            elif isinstance(value, np.ndarray):
                if value.shape[0] != batch_size:
                    raise 'Number of input {} must equal to batch size'.format(name)
            else:
                raise 'Input {} type {} cannot convert to tensor proto, only list, tuple or np.array supported'.format(
                    name, type(value))
            proto = tf.compat.v1.make_tensor_proto(value)
            if not proto.tensor_content:
                print('Warning: tensor content is null, now casting to bytes, '
                      'name={}, dtype={}'.format(name, proto.dtype))
                # make sure tensor proto is in bytes format
                proto.tensor_content = tf.make_ndarray(proto).tostring()
                if proto.dtype == types_pb2.DT_HALF:
                    proto.ClearField('half_val')
                elif proto.dtype == types_pb2.DT_STRING:
                    proto.ClearField('string_val')
                elif proto.dtype == types_pb2.DT_BOOL:
                    proto.ClearField('bool_val')

            request.inputs[name].CopyFrom(proto)

    @staticmethod
    def _prepare_outputs(request, outputs):
        assert isinstance(outputs, (list, dict, str))
        if isinstance(outputs, list):
            for o in outputs:
                output = predict_pb2.PredictRequest.Output()
                request.output_filter[o].CopyFrom(output)
        elif isinstance(outputs, dict):
            for name, output in outputs.items():
                if isinstance(output, predict_pb2.PredictRequest.Output):
                    request.output_filter[name].CopyFrom(output)
                elif isinstance(output, int):
                    request.output_filter[name].cls.count = output
                elif isinstance(output, dict) and isinstance(output.get('cls'), int):
                    request.output_filter[name].cls.count = output.get('cls')
                else:
                    raise Exception(
                        'Invalid output type, name={}, type={}, should be list or dict'.format(name, output))
        elif isinstance(outputs, str):
            request.output_filter[outputs] = predict_pb2.PredictRequest.Output()
        else:
            raise Exception('Invalid outputs type, outputs={}, should be string, list or dict'.format(outputs))

    def process_response(self, output_names, response):
        assert (response.model_spec.name == self._model_name)
        if self._model_version is not None:
            if isinstance(self._model_version, str):
                assert (response.model_spec.version_label == self._model_version)
            elif isinstance(self._model_version, int):
                assert (response.model_spec.version.value == self._model_version)
        result = {}
        for name in output_names:
            result[name] = _Output(response.outputs[name])
        return result

    def _make_model_spec(self):
        model_spec = model_spec_pb2.ModelSpec()
        model_spec.name = self._model_name
        if self._model_version is not None:
            if isinstance(self._model_version, str):
                model_spec.version_label = self._model_version
            elif isinstance(self._model_version, int):
                model_spec.version.value = self._model_version
        if self._signature is not None:
            model_spec.signature_name = self._signature
        return model_spec
