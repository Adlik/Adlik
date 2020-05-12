#!/bin/bash -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARCHIVES_DIR=/home/john/archives
ARCHIVES_DIR=/home/john/temp
$SCRIPT_DIR/download_archives.sh $ARCHIVES_DIR

export TEST_TMPDIR=.cache

printenv

# build ml dlib runtime
bazel build -c opt -j 4 --distdir=$ARCHIVES_DIR --verbose_failures --sandbox_debug  --config=tensorflow-cpu  //adlik_serving \
  --remote_http_cache=http://10.86.106.2/apps/bazel-cache \
  --incompatible_no_support_tools_in_action_inputs=false \
  --incompatible_disable_nocopts=false


# build ml task client
bazel build -c opt --distdir=$ARCHIVES_DIR --verbose_failures --config=tensorflow-cpu //adlik_serving/clients/python:build_pip_package \
  --remote_http_cache=http://10.86.106.2/apps/bazel-cache \
  --incompatible_no_support_tools_in_action_inputs=false \
  --incompatible_disable_nocopts=false
