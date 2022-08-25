# Deep recommendation model deployment inference demo (PyTorch model)

## Compile the model

1. Train the model and save as .pth file:

    Model: ENMF. <https://github.com/chenchongthu/ENMF>
    Dataset: ml-100k.
    Model source ：RecBole. (An open source recommender system library)

    ```sh
    pip install recbole
    ```

    Please refer to <https://recbole.io/docs/index.html> for a detailed description.

    ```sh
    python3 train_model.py
    ```

    After running this script, there is a .pth file in the “model” directory.

    ```text
    ├── model
    │   └── ENMF-current_time.pth
    ```

2. Convert the .pth file to .onnx file

    ```sh
    python3 pth2onnx.py
    ```

   After running this script, there is a .onnx file in the “model” directory.

    ```text
    ├── model
    │   └── ENMF.onnx
    ```

3. Pull the compiler docker image

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.5.0_trt7.2.1.6_cuda11.0
    ```

4. Run the image

   Run the follwing command:

    ```sh
    docker run -it --rm -v $PWD:/home/john/model \
    registry.cn-beijing.aliyuncs.com/adlik/model-compiler:v0.5.0_trt7.2.1.6_cuda11.0 bash
    ```

5. Convert the .onnx file to OpenVINO serving model (see [compile_model.py](./compile_model.py):

    In the docker container:

    ```sh
    cd /home/john/model/
    python3 compile_model.py
    ```

    After that there is a serving model in the “model_repos” directory:

    ```text
    |-- model_repos
    |   |-- ENMF
    |   |   |-- 1
    |   |   |   |-- TFVERSION
    |   |   |   |-- saved_model.pbtxt
    |   |   |   `-- variables
    |   |   |       |-- variables.data-00000-of-00001
    |   |   |       `-- variables.index
    |   |   `-- config.pbtxt
    |   `-- ENMF.zip
    ```

    > Note:
    >
    > 1. You also can modify the `serving_type` in [compile_model](compile_model.py) to compile the model to `tensorrt`.

## Deploy a openvino serving service

1. Pull the image for openvino serving

    ```sh
    docker pull registry.cn-beijing.aliyuncs.com/adlik/serving-openvino:v0.5.0
    ```

2. In the `Adlik/examples/recommend_model` directory, run the command:

    ```sh
    docker run -d -p 8500:8500 -v $PWD/model_repos:/srv/adlik-serving registry.cn-beijing.aliyuncs.com/adlik/serving-openvino:v0.5.0
    ```

## Do inference

1. Install the `Adlik serving` package

    ```sh
    wget https://github.com/Adlik/Adlik/releases/download/v0.3.0/adlik_serving_api-0.3.0-py2.py3-none-any.whl

    python3 -m pip install adlik_serving_api-0.3.0-py2.py3-none-any.whl
    ```

2. Run a client and do inference:

    ```sh
     python3 recommend_client.py -m ENMF -UID 62
    ```

    -m: model name  -UID: user's external id

    After that you can get the score, internal ID and external ID of the top 10 movies to recommend to that user.

    ```sh
    Run model ENMF predict (batch size=1) use 102.81515121459961 ms
    topk_score:  tensor([[0.8261, 0.8190, 0.8150, 0.8108, 0.8108, 0.8043, 0.8006, 0.7888, 0.7831, 0.7789]])
    topk_iid_list:  tensor([[ 69,  39,  85,  71,  64, 167,  10,  99,  75,  55]])
    external_item_list:  [['15' '32' '625' '416' '919' '216' '86' '328' '1444' '679']]
    ```
