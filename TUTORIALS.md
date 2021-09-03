# TUTORIALS

## A simple example by using provided docker image

This example demonstrates how to use Adlik to compile a serving model and deploy a serving service end to end.

### Compile the model

1. Pull the compiler docker image

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.3.0_trt7.2.1.6_cuda10.2
    ```

2. Use the example code to train a simple keras model:

    ```sh
    git clone https://github.com/Adlik/Adlik.git

    cd Adlik/examples/keras_model
    
    python3 train_model.py
    ```

   Then, a .h5 file will be generated in the “model” directory.

3. Run the image

   Run the follwing command:

    ```sh
    docker run -it --rm -v $PWD:/home/john/model
    registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.3.0_trt7.2.1.6_cuda10.2 bash
    ```

4. Compile the model: convert the .h5 file to TensorFlow serving model

   In the docker container:

    ```sh
    cd /home/john/model/

    python3 compile_model.py
    ```

   After that there will be a serving model in the `model_repos` directory

### Deploy TensorFlow serving service

1. Pull the image for TensorFlow serving

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/serving-tensorflow-cpu:v0.3.0
    ```

2. In the `Adlik/examples/keras_model` directory, run the command:

    ```sh
    docker run -d -p 8500:8500 -v $PWD/model_repos:/srv/adlik-serving
    registry.cn-beijing.aliyuncs.com/adlik/serving-tensorflow-cpu:v0.3.0
    ```

### Do inference

1. Install the `Adlik serving` package

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.3.0-py2.py3-none-any.whl
    ```

2. Infer with the provided client script:

   Adlik serving provides grpc and http interfaces, grpc interface mainly supports model inference and model management,
   and http interface mainly supports model inference. For detailed interface description, refer to [adlik_serving_apis.md](adlik_serving/clients/adlik_serving_apis.md).

   Examples of model inference are as follows:

   Infer with the python script.

   In the `Adlik/examples/keras_model` directory, run the command:

    ```sh
    python3 mnist_client.py data/0.png
    ```

   The result will be as follows:

    ```text
    Image: 'data/0.png', result: [{'idx': 0, 'score': 0.9999995231628418, 'label': ''}]
    ```

   Infer with the cpp script.

    ```sh
    ./cpp_client
    ```

   Currently, we provide both bazel and cmake to build cpp client. Please refer to
   [adlik_serving/clients/cpp/README.md](adlik_serving/clients/cpp/README.md) for instructions.

## More

Refer to the above example, you can also convert PyTorch, Keras, TensorFlow models to OpenVINO, TensorFlow Lite, TVM,
TF-TRT and TensorRT serving model and deploy these serving models easily with our provided [images](README.md#Docker-images).
