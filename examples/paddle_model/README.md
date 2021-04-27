# PaddlePaddle model

1. Convert PaddlePaddle model to ONNX format, run the example code as below to get a ResNet50 model(ONNX format)
```
python3 paddle_resnet50.py
```

> We can also use [Paddle2ONNX](https://github.com/PaddlePaddle/Paddle2ONNX) to convert a PaddlePaddle Inference model to ONNX format

2. Convert the ONNX format file to OpenVINO Serving model
```
python3 compile_model.py
```
After that there is a serving model in the `model_repos` directory:
```
|-- model_repos
|   |-- 
|   |   |-- 1
|   |   |   |-- TFVERSION
|   |   |   |-- saved_model.pbtxt
|   |   |   `-- variables
|   |   |       |-- variables.data-00000-of-00001
|   |   |       `-- variables.index
|   |   `-- config.pbtxt
|   `-- mnist.zip
```
> **Note:** You also can modify the `serving type` in [compile_model.py]() to compile the model to tensorrt.

4. Deploy a serving service

```
docker run -it --rm -p 8500:8500 -v model_repos:/model adlik/serving-openvino:latest bash
```

5. Run a client and do inference
```
python3 resnet50_client.py data/cat.png
```
We will get result as below
```
```
