# TopsInference Demo

## A simple example by using provided docker image

This example demonstrates how to use Adlik to compile a serving model and deploy a serving service end to end.

## Installing prerequisites

- python3
- pip
- protobuf==3.19.4
- opencv_python
- pillow

## Build and install packages

1. Build clients and serving binary and make client pip packages (see [README.md](../../README.md)).

2. Install model_compiler:

   ```sh
   cd {Adlik_root_dir}/model_compiler
   pip3 install .
   ```

## Running the sample

1. Train the model and save as .onnx file:

    Download this code.
    Adlik/benchmark/tests/test_model/resnet50_pytorch/resnet50_pytorch.py

    ```sh
    python3 resnet50_pytorch.py
    ```

    After running this script, there is a .onnx file in the “model” directory.

    ```text
    ├── model
    │   └── resnet50.onnx
    ```

2. Convert the .onnx file to Topsinference serving model
which show how to build a request about compiling models):

    ```sh
    python3 compile_model.py
    ```

    After that there is a serving model in the “model_repos” directory:

    ```text
    model_repos
    ├── resnet50
    │   ├── 1
    │   │   └── model.exec
    │   └── config.pbtxt
    └── resnet50_1.zip
    ```

    Sample of config.pbtxt

    ```text
    name: "resnet50"
    platform: "topsinference"
    platform_version: "major.minor.build"
    max_batch_size: 128
    input {
      name: "input.1"
      data_type: DT_FLOAT
      format: FORMAT_NCHW
      dims: 3
      dims: 224
      dims: 224
    }
    output {
      name: "495"
      data_type: DT_FLOAT
      dims: 1000
    }
    ```

    Adding multiple instances

    ```text
    instance_group {
      name:"resnet50"
      count : 2
    }
    ```

## Deploy TopsInference serving service

1. Pull the image for TopsInference serving

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/serving-topsinference:v0.6.0
    ```

2. In the `Adlik/examples/topsinference_model` directory, run the command:

    ```sh
    docker run -d -p 8500:8500 --device=/dev/dtu0 -v $PWD/model_repos:/srv/adlik-serving adlik/serving-topsinference:v0.6.0
    ```

## Do inference

1. Install the `Adlik serving` package

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.4.0/adlik_serving_api-0.4.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.4.0-py2.py3-none-any.whl
    ```

2. Infer with the provided client script:

   In the `Adlik/examples/topsinference_model` directory, run the command:

    ```sh
    python3 resnet50_client_acc.py -t data/val_map.txt data/figures/tench.JPEG data/imagenet_class.txt
    ```

   The result will be as follows:

    ```text
    Image: 'data/figures/tench.JPEG', result: [{'idx': 0, 'score': 14.775530815124512, 'label': '0 tench'}]
    ```
