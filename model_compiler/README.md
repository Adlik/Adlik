# Model Compiler

1. Support optimization for models from different kinds of deep learning architecture, e.g. TensorFlow/Caffe/PyTorch.

2. Support compiling models for different runtime, e.g. OpenVINO IR/TensorFlow Serving/TensorFlow Lite/TensorRT.

3. Simplified interfaces for the workflow.

## Usage

1. Prepare a model file in the specified format, such as `H5`, `Checkpoint`, `Frozen Graph`, `ONNX`.

2. Create a json file which must match [config_schema.json](config_schema.json).

3. Install model_compiler and compile the model, you can refer to
[benchmark link](../benchmark/src/compile_model.py) or [examples link](../examples/keras_model/compile_model.py):

```sh
cd {Adlik_root_dir}/model_compiler
python3 -m pip install .
```
