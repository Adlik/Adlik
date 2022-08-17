# PaddlePaddle Demo

## A simple example by using provided docker image

This example demonstrates how to use Adlik to compile a serving model and deploy a serving service end to end.

### Compile the model

1. Pull the compiler docker image

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.5.0_trt7.2.1.6_cuda11.0
    ```

2. Use the example code to get a simple paddle model:

    ```sh
    python3 -m pip install paddlepaddle==2.3.1
    python3 -m pip install paddlehub==2.2.0

    git clone https://github.com/Adlik/Adlik.git

    cd Adlik/examples/paddle_paddle

    python3 paddle_resnet50.py
    ```

   Then, ".pdmodel", ".pdiparams" and ".pdiparams.info" files will be generated in the “model_resnet50” directory.

3. Run the image

   Run the follwing command:

    ```sh
    docker run -it --rm -v $PWD:/home/john/paddle_paddle \
    registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.5.0_trt7.2.1.6_cuda11.0 bash
    ```

4. Compile the model: convert the model files to Paddle serving model

   In the docker container:

    ```sh
    cd /home/john/paddle_paddle/

    python3 compile_model_resnet50.py
    ```

   After that there will be a serving model in the `model_repos_resnet50` directory

### Deploy Paddle serving service

1. Pull the image for Paddle serving

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/serving-paddle:v0.5.0
    ```

2. In the `Adlik/examples/paddle_paddle` directory, run the command:

    ```sh
    docker run -d -p 8500:8500 -v $PWD/model_repos_resnet50:/srv/adlik-serving \
    registry.cn-beijing.aliyuncs.com/adlik/serving-paddle:v0.5.0
    ```

### Do inference

1. Install the `Adlik serving` package

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.3.0-py2.py3-none-any.whl
    ```

2. Infer with the provided client script:

   In the `Adlik/examples/paddle_paddle` directory, run the command:

    ```sh
    python3 resnet50_client_acc.py data/cat.jpg data/labels.txt
    ```

   The result will be as follows:

    ```text
    Image: 'data/cat.jpg', result: [{'idx': 281, 'score': 11.564537048339844, 'label': 'tabby'}]
    ```
