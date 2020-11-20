#!/usr/bin/env bash

model_dir="/model_repos/compiled_model"
http_port=$SERVABLE_HTTP_PORT
grpc_port=$SERVABLE_GRPC_PORT
model_name=$MODEL_NAME

echo $model_dir
echo $http_port
echo $grpc_port
echo $model_name


if [ -z $http_port ] && [ -z $grpc_port ];then
    echo "can't find the SERVABALE_HTTP_PORT or SERVABLE_GRPC_PORT"
    echo "the grpc port will be set as 8500 and http port will be set as 8501"
fi

#confirm the cmd to run servinglite server
cmd_run_server="adlik-serving --model_base_path=${model_dir} "
if [ $http_port ];then
    cmd_run_server="$cmd_run_server --http_port=${http_port}"
elif [ $grpc_port ]; then
    cmd_run_server="$cmd_run_server --grpc_port=${grpc_port}"
else
    cmd_run_server="adlik-serving --model_base_path=/srv/adlik-serving --grpc_port=8500 --http_port=8501"
fi

#start the infer server
eval $cmd_run_server

if [ "$?" != "0" ];then
    echo "servinglite server abort"
    exit 1
fi
