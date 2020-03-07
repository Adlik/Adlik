#!/usr/bin/env bash
cd /home/john/Adlik/bazel-bin/adlik_serving && \
./adlik_serving --model_base_path=/home/john/Adlik/model/model_repos --grpc_port=8500 --http_port=8501 >> /home/john/Adlik/serving_time.log 2>&1