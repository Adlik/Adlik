# About this sample

This sample demonstrates how to use Adlik to compile serving model and deploy serving service end to end.

## Installing prerequisites

- python3
- pip

## Build and install packages

1. Build clients and serving binary and make client pip packages (see [README.md](../../README.md)).

2. Install clients pip package:

   ```sh
   pip3 install {dir_of_pip_package}/adlik_serving_api-0.0.0-py2.py3-none-any.whl
   ```

3. Install model_compiler:

   ```sh
   cd {Adlik_root_dir}/model_compiler
   pip3 install .
   ```

## Running the sample

1. Train the model and save as .h5 file:

    ```sh
    python3 train_model.py
    ```

    After running this script, there is a .h5 file in the “model” directory.

    ```text
    ├── model
    │   └── mnist.h5
    ```

2. Convert the .h5 file to TensorFlow serving model (see [compile_model.py](./compile_model.py) and [config_schema.json](../../model_compiler/config_schema.json)
which show how to build a request about compiling models):

    ```sh
    python3 compile_model.py
    ```

    After that there is a serving model in the “model_repos” directory:

    ```text
    |-- model_repos
    |   |-- mnist
    |   |   |-- 1
    |   |   |   |-- TFVERSION
    |   |   |   |-- saved_model.pbtxt
    |   |   |   `-- variables
    |   |   |       |-- variables.data-00000-of-00001
    |   |   |       `-- variables.index
    |   |   `-- config.pbtxt
    |   `-- mnist.zip
    ```

    >Note:
    >
    >1. You also can modify the `serving_type` in [compile_model](compile_model.py) to compile the model to other types,
    such as `openvino`, `tensorrt`.
    >
    >2. Before you compile the model you need to build the corresponding type of serving binary.

3. Deploy a serving service:

    ```sh
    cd {dir_of_adlik_serving_binary}
    ./adlik_serving --model_base_path={model_repos_dir} --grpc_port={grpc_port} --http_port={http_port}
    ```

    In this sample, we run service like this, and the `grpc_port=8500` mean use grpc protocol and the port is 8500, the
    `http_port=8501` mean use http protocol and the port is 8501. And set up a protocol port is ok.

    ```sh
    ./adlik_serving --model_base_path=/Adlik/samples/simpleMNIST/model_repos --grpc_port=8500 --http_port=8501
    ```

4. Run a client and do inference:

    ```sh
     python3 mnist_client.py /Adlik/samples/simpleMNIST/data/0.png
    ```

    The result is:

    ```text
    Run model mnist predict (batch size=1) use 2.122163772583008 ms
    Image: '/Adlik/samples/simpleMNIST/data/0.png', result: [{'idx': 0, 'score': 0.9999997615814209, 'label': ''}]
    ```
