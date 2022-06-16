# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Automated test runtime performance.
"""

import argparse
import os
import subprocess


def _parse_arguments():
    args_parser = argparse.ArgumentParser()
    args_parser.add_argument("-d", "--docker-file-path", type=str, required=True,
                             help="The docker file path of the test serving type")
    args_parser.add_argument("-s", "--serving-type", type=str, required=True, help="The test serving type",
                             choices=("openvino", "tensorrt", "tensorflow", "tensorflow_gpu", "tensorflow_lite"))
    args_parser.add_argument("-b", "--build-directory", type=str, required=True,
                             help="The directory which to build the docker")
    args_parser.add_argument("-a", "--adlik-directory", type=str, default="Adlik", help="The adlik directory")
    args_parser.add_argument("-m", "--model-name", type=str, required=True, help="The path of model used for test")
    args_parser.add_argument("-c", "--client-script", type=str, default="client_script.sh",
                             help="The script used to infer")
    args_parser.add_argument("-ss", "--serving-script", type=str, default="serving_script.sh",
                             help="The serving script")
    args_parser.add_argument("-ov", "--openvino-version", type=str, default="2021.1.0",
                             help="The version of the OpenVINO")
    args_parser.add_argument("-tt", "--tensorrt-tar", type=str,
                             default="TensorRT-7.1.3.4.Ubuntu-18.04.x86_64-gnu.cuda-10.2.cudnn8.0.tar.gz",
                             help="The tar version of the TensorRT")
    args_parser.add_argument("-tv", "--tensorrt-version", type=str, default="7.1.3.4", help="The version of TensorRT")
    args_parser.add_argument("-l", "--log-path", type=str, default="log", help="The path of log directory")
    args_parser.add_argument('-tm', '--test-model-path', type=str, required=True, help="The path of test model")
    args_parser.add_argument("-sj", "--serving-json", type=str, default="serving_model.json", help="The json of model")
    args_parser.add_argument("-cis", "--client-inference-script", type=str, required=True, help="The inference script")
    args_parser.add_argument("-i", "--image-filename", type=str, required=True, nargs="?", help="Input image.")
    args_parser.add_argument("-gl", "--gpu-label", type=str, default=None, help="The GPU label")
    args_parser.add_argument("-cs", "--compile-script", type=str, default="compile_script.sh",
                             help="Compile the model script")
    return args_parser.parse_args()


def _get_result(log_path, model_name):
    calculate_command = ['python3', os.path.join(os.path.dirname(__file__), 'test_result.py'),
                         '-c', os.path.join(log_path, 'client_time.log'),
                         '-s', os.path.join(log_path, 'serving_time.log'),
                         '-m', model_name]
    with subprocess.Popen(calculate_command, stdout=subprocess.PIPE, universal_newlines=True) as result_process:
        print(result_process.stdout.read())


def _docker_build_command(args):
    build_arg = ['--build-arg', f'MODEL_NAME={args.model_name}',
                 '--build-arg', f'SERVING_SCRIPT={args.serving_script}',
                 '--build-arg', f'CLIENT_SCRIPT={args.client_script}',
                 '--build-arg', f'TEST_MODEL_PATH={args.test_model_path}',
                 '--build-arg', f'SERVING_JSON={args.serving_json}',
                 '--build-arg', f'CLIENT_INFERENCE_SCRIPT={args.client_inference_script}',
                 '--build-arg', f'IMAGE_FILENAME={args.image_filename}',
                 '--build-arg', f'COMPILE_SCRIPT={args.compile_script}']

    if args.serving_type == 'openvino':
        build_arg.extend(['--build-arg', f'OPENVINO_VERSION={args.openvino_version}'])
    elif args.serving_type == 'tensorrt':
        build_arg.extend(['--build-arg', f'TENSORRT_VERSION={args.tensorrt_version}',
                          '--build-arg', f'TENSORRT_TAR={args.tensorrt_tar}'])
    else:
        build_arg = build_arg

    build_command = ['docker', 'build', '--build-arg', f'ADLIK_DIRECTORY={args.adlik_directory}']
    build_command.extend(build_arg)
    build_command.extend(['-f', f'{args.docker_file_path}'])
    build_command.extend(['-t', f'adlik-test:{args.serving_type}', f'{args.build_directory}'])
    return build_command


def main(args):
    """
    Automated test runtime performance.
    """

    docker_build_command = _docker_build_command(args)

    env = os.environ.copy()
    if args.gpu_label is None:
        docker_run_command = ['docker', 'run', '--rm',
                              '-v', f'{args.log_path}:/home/john/log',
                              f'adlik-test:{args.serving_type}']
    else:
        docker_run_command = ['nvidia-docker', 'run', '--rm',
                              '-v', f'{args.log_path}:/home/john/log',
                              f'adlik-test:{args.serving_type}']

        env['NV_GPU'] = args.gpu_label
    subprocess.run(docker_build_command, check=True)
    subprocess.run(docker_run_command, check=True, env=env)
    _get_result(args.log_path, args.model_name)


if __name__ == '__main__':
    main(_parse_arguments())
