# About the Cloud Native Image

Cloud native Image is used to provide Adlik model compiler and inference engine public dockerfiles, including model
compiler dockerfile, OpenVINO serving dockerfile, TFServing-CPU dockerfile, TFLite-CPU dockerfile, TFServing-GPU
dockerfile and TensorRT dockerfile.

All dockerfiles are tested by local mirroring and kubernetes + mirroring, which can achieve different format model
compilation and different inference engine services and inference calculation functions.

The test models include Yolov3 (.h5 model file and .onnx model file), Resnet50 (.h5 model file),
Bert (.pb model file and checkpoint model file).

Yolov3 model based on trained weights and config：

config: [yolov3.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3.cfg)

weights: [yolov3.weights](https://pjreddie.com/media/files/yolov3.weights)

The H5 model of Yolov3 is converted by [convert.py](https://github.com/qqwweee/keras-yolo3/blob/master/convert.py)
based on the [yolov3.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3.cfg) and [yolov3.weights](https://pjreddie.com/media/files/yolov3.weights).

The ONNX model of Yolov3 is converted by [yolov3_to_onnx.py](https://github.com/NVIDIA/TensorRT/blob/master/samples/python/yolov3_onnx/yolov3_to_onnx.py)
based on the [yolov3.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3.cfg) and [yolov3.weights](https://pjreddie.com/media/files/yolov3.weights).

Resnet50 model is generated with [resnet50_keras.py](https://github.com/Adlik/Adlik/blob/master/benchmark/tests/test_model/resnet50_keras/resnet50_keras.py).

The BERT model can be obtained from this [page](https://github.com/google-research/bert).

Among them, The CKPT model of Bert can download get the .zip file, [BERT-Base, Chinese](https://storage.googleapis.com/bert_models/2018_11_03/chinese_L-12_H-768_A-12.zip).

The PB model of Bert is converted by [freeze_graph.py](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/python/tools/freeze_graph.py).

## Model compiler image

Model compiler provides a variety of format model compilation services.

Use requirements: 1. Linux environment. 2. The trained model, the model file format can be .h5, checkpoint, .pb,
.onnx, saved model. 3. Configure the environment variables or json files needed for the compiled model.

Steps for usage:

1. Execute [build.py](https://github.com/Adlik/Adlik/blob/master/docker-images/build.py) to generate the model-compiler
   image.

2. Run the image.

   ```shell script
   docker run -it --rm -v /media/B/work/source_model:/mnt/model 
   registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.5.0_trt7.2.1.6_cuda11.0 bash
   ```

3. Configure the json file or environment variables required to compile the model.

   For the description of the json file field information, see [config_schema.json](https://github.com/Adlik/Adlik/blob/master/model_compiler/config_schema.json),
   for the example, reference [compiler_json_example.json](https://github.com/Adlik/Adlik/blob/master/docker-images/compiler_json_example.json).
   For the environment variable field description, see [env_field.txt](https://github.com/Adlik/Adlik/blob/master/docker-images/env_field.txt),
   for the example, reference [compiler_env_example.txt](https://github.com/Adlik/Adlik/blob/master/docker-images/compiler_env_example.txt).

   Note: The checkpoint model must be given the input and output op names of the model when compiling, and other models
         can be compiled without the input and output op names of the model.

4. Compile the model.

   Compilation instructions (json file mode):

   ```shell script
   python3 "-c" "import json; import model_compiler as compiler;file=open('/mnt/model/serving_model.json','r');
   request=json.load(file);compiler.compile_model(request);file.close()"
   ```

   Compilation instructions (environment variable mode):

   ```shell script
   python3 "-c" "import model_compiler.compiler as compiler;compiler.compile_from_env()"
   ```

### NOTE

There are multiple options for TensorRT version and CUDA version in the model-compiler image file, which can be
customized by users. Refer to the following table for the correspondence between the two versions. The compiled model
used by the TensorRT inference engine for service. The TensorRT version used by this model during compilation must be
consistent with the TensorRT version in the TensorRT inference engine image.

|   TensorRT  |    CUDA    |
| ----------- | :--------: |
|   7.0.0.*   |    10.0    |
|   7.0.0.*   |    10.2    |
|   7.1.3.*   |    10.2    |
|   7.1.3.*   |    11.0    |
|   7.2.0.*   |    11.0    |
|   7.2.1.*   |    10.2    |
|   7.2.1.*   |    11.0    |

## Inference engine image

The inference engine pulls the compiled model, starts the service and performs inference operations.

Use requirements: 1. Linux environment. 2. Compiled intermediate model file. 3. User-defined client code and
wheel package.

Steps for usage:

1. Execute [build.py](https://github.com/Adlik/Adlik/blob/master/docker-images/build.py) to generate the inference
   engine images.

2. Run the mirror and pay attention to mapping out the service port.

    ```shell script
    docker run -it --rm -p 8500:8500 -v /media/B/work/compiled_model:/model
    registry.cn-beijing.aliyuncs.com/adlik/serving-openvino:v0.3.0 bash
    ```

3. Load the compiled model in the image and start the service.

    ```shell script
    adlik-serving --grpc_port=8500 --http_port=8501 --model_base_path=/model
    ```

4. Install the client wheel package [adlik serving package](
   https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl) or [adlik
   serving gpu package](
      https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api_gpu-0.3.0-py2.py3-none-any.whl) locally.

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.3.0-py2.py3-none-any.whl
    ```

5. Infer with the provided client script, The script used in this example is [yolov3_client.py](https://github.com/Adlik/Adlik/blob/master/benchmark/tests/client/yolov3_client.py).
   For more client model inference script examples, please refer to [client](https://github.com/Adlik/Adlik/tree/master/benchmark/tests/client).

    ```sh
    python3 yolov3_client.py data/dog.png
    ```

Note: If the service port is not mapped when you run the mirror, you need install the [adlik serving package](
   https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl) or [adlik
   serving gpu package](
      https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api_gpu-0.3.0-py2.py3-none-any.whl) in the
      container. Then execute the inference code, and perform inference in the container.
