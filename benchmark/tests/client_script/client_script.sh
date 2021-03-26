#!/usr/bin/env bash
python3 /home/john/Adlik/benchmark/tests/client/$CLIENT_INFERENCE_SCRIPT --batch-size=128 /home/john/Adlik/benchmark/tests/data/$IMAGE_FILENAME 1>>/home/john/Adlik/client_time.log && \
mv /home/john/Adlik/client_time.log /home/john/log/client_time.log && \
mv /home/john/Adlik/serving_time.log /home/john/log/serving_time.log