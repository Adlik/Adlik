# About the benchmark

The benchmark is used to test the Adlik serving performance of different models.

## Test the runtime performance

The parameters of the automatic test framework are as follows:

| abbreviation | detail                    | type | help                                          | default                                                             |
| ------------ | ------------------------- | ---- | --------------------------------------------- | ------------------------------------------------------------------- |
| -d           | --docker-file-path        | str  | The docker file path of the test serving type |                                                                     |
| -s           | --serving-type            | str  | The test serving type                         |                                                                     |
| -b           | --build-directory         | str  | The directory which to build the docker       |                                                                     |
| -a           | --adlik-directory         | str  | The adlik directory                           | Adlik                                                               |
| -m           | --model-name              | str  | The path of model used for test               |                                                                     |
| -c           | --client-script           | str  | The script used to infer                      | client_script.sh                                                    |
| -ss          | --serving-script          | str  | The serving script                            | serving_script.sh                                                   |
| -ov          | --openvino-version        | str  | The version of the OpenVINO                   | 2020.3.194                                                          |
| -tt          | --tensorrt-tar            | str  | The tar version of the TensorRT               | TensorRT-7.0.0.11.Ubuntu-18.04.x86_64-gnu.cuda-10.0.cudnn7.6.tar.gz |
| -tv          | --tensorrt-version        | str  | The version of TensorRT                       | 7.0.0.11                                                            |
| -l           | --log-path                | str  | The path of log directory                     | log                                                                 |
| -tm          | --test-model-path         | str  | The path of test model                        |                                                                     |
| -sj          | --serving-json            | str  | The json of model                             | serving_model.json                                                  |
| -cis         | --client-inference-script | str  | The inference script                          |                                                                     |
| -i           | --image-filename          | str  | Input image                                   |                                                                     |
| -gl          | --gpu-label               | int  | The GPU label                                 | None                                                                |
| -cs          | --compile-script          | str  | Compile the model script                      | compile_script.sh                                                   |

If you want to use the automatic_test.py test the runtime, you need to follow the steps below:

1. Download Adlik code.
2. Prepare the serving_model.json (required for compiling model) trained model, the format of the model file can be: .pb,
.h5, .ckpt, .onnx, savedModel, and it is recommended to put the model and serving_model.json under the
Adlik/benchmark/tests/test_model directory.
3. The writing of serving_model.json can refer to the serving_model.json of each model in
Adlik/benchmark/tests/test_model and Adlik/model_compiler/src/model_compiler/config_schema.json.
4. The absolute paths of "export_path" in serving_model.json and "--model_base_path" in serving_script.sh need to be consistent.
5. If there is no required inference code in the Adlik/benchmark/tests/client directory of the benchmark, you need to
write the inference code.
6. Specify the type of test runtime and version number (if needed, eg: OpenVINO and TensorRT).
7. Explicitly test whether a GPU is required.
8. The environment running the code has python3.7 or above installed.
9. According to the runtime type of the test, select the dockerfile, serving script and compile script required by the
test under the Adlik/benchmark/tests directory.
10. Configure parameters for testing, for example, run the follow command in the Adlik directory:

```sh

python3 benchmark/src/automatic_test.py -d benchmark/tests/docker_test/openvino.Dockerfile -s openvino -b . -a . -m
mnist -c benchmark/tests/client_script/client_script.sh -ss benchmark/tests/serving_script/openvino_serving_script.sh -l
abspath(log) -tm benchmark/tests/test_model/mnist_keras -cis mnist_client.py -i mnist.png -cs
benchmark/tests/compile_script/compile_script.sh
```

## NOTE

1. If the test tensorrt is running, you need to register and download the dependency package from
[TensorRT](https://docs.nvidia.com/deeplearning/sdk/tensorrt-install-guide/index.html). The downloaded dependency
package is recommended to be placed in the Adlik directory.
2. If your local environment has no way to connect to the external network, you need to configure the apt source and pip
source that can be used, and add the configuration command to the Dockerfile.
3. When bazel build, if you can't pull the package, you can also download the required packages in advance, and use the
--distdir command.
4. To prevent the computer from occupying too many cores when bazel build, causing jams. During bazel build, you can
also use --jobs to set concurrent jobs.

## Inference performance of Adlik

At present, the inference performance test results of Adlik serving engine on different models include: the test result
of the MNIST model, the test result of the ResNet50 model, and the test result of the InceptionV3 model.The CPU and GPU
parameters used in the test are as follows:

|     |                   type                    | number |
| --- | :---------------------------------------: | :----: |
| CPU | Intel(R) Xeon(R) CPU E5-2680 v4 @ 2.40GHz |   1    |
| GPU |           Tesla V100 SXM2 32GB            |   1    |

### The test result of the MNIST model

#### The test result of MNIST model in Keras format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| TF Serving1.14 `CPU` |            2200.372            |                2291.584                |             1.81E-05              |
| TF Serving2.1  `CPU` |            2003.901            |                2077.644                |             1.77E-05              |
| OpenVINO       `CPU` |            2647.344            |                2788.087                |             1.90E-05              |
| TFLite         `CPU` |            778.890             |                790.150                 |             1.82E-05              |
| TF Serving1.14 `GPU` |           19414.208            |               52247.525                |             3.24E-05              |
| TF Serving2.1  `GPU` |           19395.129            |               53640.456                |             3.29E-05              |
| TensorRT       `GPU` |           37342.687            |               163058.592               |             2.06E-05              |

#### The test result of MNIST model in Tensorflow format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| TF Serving1.14 `CPU` |            1486.878            |                1531.337                |             1.95E-05              |
| TF Serving2.1  `CPU` |            2160.331            |                2248.311                |             1.81E-05              |
| TFLite         `CPU` |            3114.246            |                3250.399                |             1.34E-05              |
| TF Serving1.14 `GPU` |           19043.582            |               51448.587                |             3.31E-05              |
| TF Serving2.1  `GPU` |           19244.343            |               50705.164                |             3.22E-05              |

#### The test result of MNIST model in Pytorch format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| OpenVINO       `CPU` |            9053.677            |               10226.869                |             1.27E-05              |
| TensorRT       `GPU` |           46318.234            |               249302.706               |             1.76E-05              |

>Note
>
>>i. The "CPU" or "GPU" listed in the first column of the table represents the different runtime environments used in
>the service engine test.
>>
>>ii. The test result is taken from the batch inference calculation result with the batch size of 128.
>>
>>iii. The test model of TensorFlow Lite is an unquantified model, and the number of threads is set to 1 during testing.

### The test result of the ResNet50 model

#### The test result of ResNet50 model in Keras format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| TF Serving1.14 `CPU` |             3.599              |                 3.640                  |              0.00311              |
| TF Serving2.1  `CPU` |             6.183              |                 6.301                  |              0.00302              |
| OpenVINO       `CPU` |             9.359              |                 9.642                  |              0.00313              |
| TFLite         `CPU` |             2.838              |                 2.862                  |              0.00298              |
| TF Serving1.14 `GPU` |            175.423             |                433.627                 |              0.00339              |
| TF Serving2.1  `GPU` |            170.680             |                420.814                 |              0.00348              |
| TensorRT       `GPU` |            237.176             |                1402.338                |              0.00350              |

#### The test result of ResNet50 model in Tensorflow format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| TF Serving1.14 `CPU` |             3.669              |                 3.711                  |              0.00305              |
| TF Serving2.1  `CPU` |             6.554              |                 6.684                  |              0.00298              |
| TFLite         `CPU` |             2.870              |                 2.895                  |              0.00296              |
| TF Serving1.14 `GPU` |            181.118             |                454.013                 |              0.00331              |
| TF Serving2.1  `GPU` |            176.710             |                473.091                 |              0.00354              |

#### The test result of ResNet50 model in Pytorch format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| OpenVINO       `CPU` |             9.274              |                 9.552                  |              0.00313              |
| TensorRT       `GPU` |            238.244             |                1332.449                |              0.00344              |

### The test result of the InceptionV3 model

#### The test result of InceptionV3 model in Keras format

|                      | speed of client (pictures/sec) | speed of serving engine (pictures/sec) | tail latency of one picture (sec) |
| -------------------- | :----------------------------: | :------------------------------------: | :-------------------------------: |
| TF Serving2.1  `CPU` |             4.622              |                 4.752                  |              0.00589              |
| TF Serving2.2  `CPU` |             3.752              |                 3.854                  |              0.00704              |
| TF Serving2.1  `GPU` |            107.667             |                291.931                 |              0.00586              |
