#!/usr/bin/env bash

WORKSPACE="/mnt/workspace"
LOG_PATH=""

if [ -d $WORKSPACE ];then
    if [ $JOB_ID ];then
        LOG_PATH="$WORKSPACE/$JOB_ID.log"
    fi
fi

echo "&&& log path is: $LOG_PATH"

python3 -c 'import model_compiler.main as main; main.main()' 2>&1 | tee $LOG_PATH