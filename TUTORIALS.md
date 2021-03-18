# TUTORIALS

## A simple example by using provided docker image

This example demonstrates how to use Adlik to compile a serving model and deploy a serving service end to end.

### Compile the model

1. Pull the compiler docker image

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/model-compiler:7.0.0.11_10.0
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
    registry.cn-beijing.aliyuncs.com/adlik/model-compiler:7.0.0.11_10.0 bash
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
    docker pull registry.cn-beijing.aliyuncs.com/adlik/serving/tensorflow-cpu:latest
    ```

2. In the `Adlik/examples/keras_model` directory, run the command:

    ```sh
    docker run -d -p 8500:8500 -v $PWD/model_repos:/srv/adlik-serving registry.cn-beijing.aliyuncs.com/adlik/serving/tensorflow-cpu:latest
    ```

### Do inference

1. Install the `Adlik serving` package

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.2.0/adlik_serving_api-0.0.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.0.0-py2.py3-none-any.whl
    ```

2. Infer with the provided client script:

   In the `Adlik/examples/keras_model` directory, run the command:

    ```sh
    python3 mnist_client.py data/0.png
    ```

   The result will be as follows:

    ```text
    Image: 'data/0.png', result: [{'idx': 0, 'score': 0.9999995231628418, 'label': ''}]
    ```

## More

Refer to the above example, you can also convert PyTorch, Keras, TensorFlow models to OpenVINO, TensorFlow Lite and
TensorRT serving model and deploy these serving models easily with our provided [images](README.md#Docker-images).
