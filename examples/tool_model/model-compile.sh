date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/alexnet_0.pb", "model_name": "alexnet_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/densenet_0.pb", "model_name": "densenet_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc74.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/googlenet_0.pb", "model_name": "googlenet_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/mnasnet_0.pb", "model_name": "mnasnet_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc20.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/mobilenetv1_0.pb", "model_name": "mobilenetv1_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/mobilenetv2_0.pb", "model_name": "mobilenetv2_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc20.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/mobilenetv3large_0.pb", "model_name": "mobilenetv3large_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc19.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/mobilenetv3small_0.pb", "model_name": "mobilenetv3small_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc15.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/proxylessnas_0.pb", "model_name": "proxylessnas_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc24.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/resnet18_0.pb", "model_name": "resnet18_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc13.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/resnet34_0.pb", "model_name": "resnet34_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc21.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/resnet50_0.pb", "model_name": "resnet50_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc21.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/shufflenetv2_0.pb", "model_name": "shufflenetv2_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc19.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/squeezenet_0.pb", "model_name": "squeezenet_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/vgg11_0.pb", "model_name": "vgg11_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/vgg13_0.pb", "model_name": "vgg13_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/vgg16_0.pb", "model_name": "vgg16_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date && python3 compile_model.py --request '{
    "serving_type": "tflite",
    "input_model": "pb_models/vgg19_0.pb", "model_name": "vgg19_0",
    "export_path": "models_repo",
    "input_names": [
        "input_im_0"
    ],
    "input_formats": [
        "channels_last"
    ],
    "output_names": [
        "fc3.fc/MatMul"
    ],
    "max_batch_size": 128
}'

date