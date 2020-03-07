#!/usr/bin/env bash
INSTALL_DIR=/opt/intel/openvino_$OPENVINO_VERSION
source $INSTALL_DIR/bin/setupvars.sh
cd /home/john/Adlik/benchmark/src &&\
python3 compile_model.py -t /home/john/Adlik/model -s $SERVING_JSON