mkdir models-serving

model_names=("alexnet_0" "densenet_0" "googlenet_0" "mnasnet_0" "mobilenetv1_0" "mobilenetv2_0" "mobilenetv3large_0" "mobilenetv3small_0" "proxylessnas_0" "resnet18_0" "resnet34_0" "resnet50_0" "shufflenetv2_0" "squeezenet_0" "vgg11_0" "vgg13_0" "vgg16_0" "vgg19_0")

for name in "${model_names[@]}";
do
    cp models_repo/$name/1/model.tflite models-serving/$name.tflite
done
