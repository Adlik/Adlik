# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess  # nosec
import sys
import xml.etree.ElementTree as xmlTree  # nosec
from typing import Any, Dict, List, Mapping, NamedTuple, Optional

from .models.data_type import DataType
from .protos.generated.model_config_pb2 import ModelInput, ModelOutput
from .utilities import split_by, get_input_shapes_from_env


class _Layer(NamedTuple):
    ports: Dict[str, tuple]
    type: str
    id: str
    name: str

    @staticmethod
    def from_xml(xml_layer):
        ports = {}
        for attr in xml_layer:
            if attr.tag == 'input':
                for port in attr:
                    ports.update(_Layer._get_input_port_from_xml(port))
            elif attr.tag == 'output':
                for port in attr:
                    ports.update(_Layer._get_output_port_from_xml(port))
        return _Layer(ports=ports,
                      type=xml_layer.attrib['type'],
                      id=xml_layer.attrib['id'],
                      name=xml_layer.attrib['name'])

    @staticmethod
    def _get_input_port_from_xml(xml_port):
        input_shape = [int(dim.text) for dim in xml_port]
        return {xml_port.attrib['id']: (input_shape,)}

    @staticmethod
    def _get_output_port_from_xml(xml_port):
        output_shape = [int(dim.text) for dim in xml_port]
        return {xml_port.attrib['id']: (output_shape, xml_port.attrib['precision'])}


class _Edge(NamedTuple):
    from_layer: str
    from_port: str
    to_layer: str
    to_port: str

    @staticmethod
    def from_xml(xml_edge):
        return _Edge(from_layer=xml_edge.attrib['from-layer'],
                     from_port=xml_edge.attrib['from-port'],
                     to_layer=xml_edge.attrib['to-layer'],
                     to_port=xml_edge.attrib['to-port'])


class ModelParser(NamedTuple):
    edges: List[_Edge] = []
    layers: List[_Layer] = []

    @staticmethod
    def from_xml(xml_path: str):
        xml_root = xmlTree.parse(xml_path).getroot()  # nosec
        layers = []
        edges = []
        # Parse XML
        for child in xml_root:
            if child.tag == 'layers':
                for layer in child:
                    layers.append(_Layer.from_xml(layer))
            elif child.tag == 'edges':
                for edge in child:
                    edges.append(_Edge.from_xml(edge))
        return ModelParser(edges=edges, layers=layers)

    def _get_inputs_info(self):
        return [(layer.name,) + layer.ports['0'] for layer in self.layers if layer.type in ('Input', 'Parameter')]

    def _find_output_info(self, layer):
        for key in layer.ports:
            for edge in self.edges:
                if edge.to_layer == layer.id and edge.to_port == key:
                    output_layer = self._find_layer_by_id(edge.from_layer)
                    return (output_layer.name,) + output_layer.ports[edge.from_port]
        raise ValueError('can not find layer by the tag "Result" layer')

    def _find_layer_by_id(self, layer_id):
        return next(layer for layer in self.layers if layer.id == layer_id)

    def _get_outputs_info(self):
        return [self._find_output_info(layer) for layer in self.layers if layer.type == 'Result']

    def get_inputs(self):
        inputs = []
        for name, shape, data_type in self._get_inputs_info():
            inputs.append(ModelInput(name=name,
                                     data_type=DataType.from_openvino_data_type(data_type).to_tf_data_type(),
                                     format=ModelInput.FORMAT_NONE,  # pylint: disable=no-member
                                     dims=[-1 if dim is None else dim for dim in shape[1:]]))
        return inputs

    def get_outputs(self):
        outputs = []
        for name, shape, data_type in self._get_outputs_info():
            outputs.append(ModelOutput(name=name,
                                       data_type=DataType.from_openvino_data_type(data_type).to_tf_data_type(),
                                       dims=[-1 if dim is None else dim for dim in shape[1:]]))
        return outputs


class Config(NamedTuple):
    input_names: Optional[List[str]] = None
    input_shapes: Optional[List[list]] = None
    output_names: Optional[List[str]] = None
    max_batch_size: Optional[int] = None
    # if set enable_nhwc_to_nchw=True, the optimizer will transform the model format from channel_last to channel_first
    enable_nhwc_to_nchw: Optional[bool] = None
    saved_model_tags: Optional[List[str]] = None

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(input_names=value.get('input_names'),
                      input_shapes=value.get('input_shapes'),
                      output_names=value.get('output_names'),
                      max_batch_size=value.get('max_batch_size'),
                      enable_nhwc_to_nchw=value.get('enable_nhwc_to_nchw'),
                      saved_model_tags=value.get('saved_model_tags'))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        input_names = split_by(env.get('INPUT_NAMES'), ',')
        input_shapes = env.get('INPUT_SHAPES')
        input_shapes = None if input_shapes is None else get_input_shapes_from_env(input_shapes)
        output_names = split_by(env.get('OUTPUT_NAMES'), ',')
        temp_max_batch_size = env.get('MAX_BATCH_SIZE')
        max_batch_size = int(temp_max_batch_size) if temp_max_batch_size else None
        # if set enable_nhwc_to_nchw to a non-zero value, the optimizer will transform the model format
        temp_enable_nhwc_to_nchw = env.get('ENABLE_NHWC_TO_NCHW')
        enable_nhwc_to_nchw = bool(int(temp_enable_nhwc_to_nchw)) if temp_enable_nhwc_to_nchw else None
        saved_model_tags = env.get('SAVED_MODEL_TAGS')

        return Config(input_names=input_names,
                      input_shapes=input_shapes,
                      output_names=output_names,
                      max_batch_size=max_batch_size,
                      enable_nhwc_to_nchw=enable_nhwc_to_nchw,
                      saved_model_tags=saved_model_tags.split(',') if saved_model_tags else None)


def get_version():
    version_txt = os.path.join(_acquire_optimizer_base_dir(), 'deployment_tools/model_optimizer/version.txt')

    try:
        file = open(version_txt)
    except FileNotFoundError:
        return 'unknown version'

    with file:
        return file.readline().rstrip()


def execute_optimize_action(params: Dict[str, str]):
    subprocess.run(_args_dict_to_list(params), check=True)  # nosec


def _args_dict_to_list(params: Dict[str, str]) -> List[str]:
    args = [sys.executable, _acquire_optimizer_script_dir(params.pop('script_name'))]
    for key, value in params.items():
        args.extend(['--' + key] if value is None else ['--' + key, value])
    return args


def _acquire_optimizer_script_dir(script_name):
    return os.path.join(_acquire_optimizer_base_dir(), 'deployment_tools/model_optimizer', script_name)


def _acquire_optimizer_base_dir():
    return os.getenv('INTEL_CVSDK_DIR', '/opt/intel/openvino_2021.1.110')
