# Pytorch model

1. Train the model and save as .h5 file:

    ```sh
    python3 train_model.py
    ```

    After running this script, there is a .onnx file in the "model" directory.

    ```text
    ├── model
    │   └── mnist.onnx
    ```

2. Convert the .onnx file to OpenVINO serving model (see [compile_model.py](./compile_model.py):

    ```sh
    python3 compile_model.py
    ```

    After that there is a serving model in the "model_repos" directory:

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
    >1. You also can modify the `serving_type` in [compile_model](compile_model.py) to compile the model to `tensorrt`.

3. Deploy a serving service:

    ```sh
    source /opt/intel/openvino_VERSION/bin/setupvars.sh
    cd {dir_of_adlik_serving_binary}
    ./adlik_serving --model_base_path=/Adlik/samples/simpleMNIST/model_repos --grpc_port=8500 --http_port=8501
    ```

4. Run a client and do inference:

    ```sh
     python3 mnist_client.py /Adlik/samples/simpleMNIST/data/0.png
    ```
