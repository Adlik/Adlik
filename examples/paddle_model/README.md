# PaddlePaddle model

## More Information

- [PaddlePaddle Official Websize](https://www.paddlepaddle.org.cn/)
- [PaddlePaddle GitHub Repo](https://github.com/paddlepaddle/paddle)
- [Paddle2ONNX GitHub Repo](https://github.com/paddlepaddle/Paddle2ONNX)

- [PaddleDetection](https://github.com/paddlepaddle/PaddleDetection): An end-to-end object detection development kit based on PaddlePaddle.
- [PaddleSeg](https://github.com/paddlepaddle/PaddleSeg): An end-to-end image segmentation kit based on PaddlePaddle.
- [PaddleClas](https://github.com/paddlepaddle/PaddleClas): A treasure chest for image classification powered by PaddlePaddle.

## Run PaddlePaddle model by Adlik

1.Convert PaddlePaddle model to ONNX format, run the example code as below to get a ResNet50 model(ONNX format)

```Shell
python3 paddle_resnet50.py
```

> We can also use [Paddle2ONNX](https://github.com/PaddlePaddle/Paddle2ONNX) to convert a PaddlePaddle Inference model to ONNX format

2.Convert the ONNX format file to OpenVINO Serving model

```Shell
python3 compile_model.py
```

After that there is a serving model in the `model_repos` directory:

```Shell
|-- model_repos
|   |-- ResNet50_vd_ssld
|   |   |-- 1
|   |   |   |-- model.bin
|   |   |   |-- model.mapping
|   |   |   `-- model.xml
|   |   `-- config.pbtxt
|   `-- ResNet50_vd_ssld_1.zip
```

**Note:** You also can modify the `serving type` in [compile_model.py](./compile_model.py) to compile the model to tensorrt.

3.Deploy a serving service

```Shell
docker run -it --rm -p 8500:8500 \
           -v model_repos:/model \
           adlik/serving-openvino:latest bash
```

4.Run a client and do inference

```Shell
python3 resnet50_client.py data/cat.png data/labels.txt
```

We will get result as below

```Shell
Image: 'data/cat.jpg', result: [{'idx': 281, 'score': 11.564541816711426,
'label': 'tabby'}]
```

