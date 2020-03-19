#!/bin/bash

set -e

function main() {
  if [[ $# -lt 1 ]] ; then
    echo "No destination dir provided."
    echo "Example usage: $0 /tmp/pip"
    exit 1
  fi

  if [[ ! -d "bazel-bin/adlik_serving" ]]; then
    echo "Could not find bazel-bin. Did you run from the root of the build"\
      "tree?"
    exit 1
  fi

  local BAZEL_PROJECT_DIR="bazel-${PWD##*/}"
  DEST="$1"
  TMPDIR="$(mktemp -d)"
  local PIP_SRC_DIR="adlik_serving/clients/task"

  echo $(date) : "=== Using tmpdir: ${TMPDIR}"
  mkdir -p ${TMPDIR}/adlik_serving/framework/domain
  mkdir -p ${TMPDIR}/adlik_serving/apis

  echo "Adding python files"
  cp bazel-bin/adlik_serving/apis/*_pb2.py \
    "${TMPDIR}/adlik_serving/apis"

  cp bazel-bin/adlik_serving/apis/*_pb2_grpc.py \
    "${TMPDIR}/adlik_serving/apis"

  cp bazel-bin/adlik_serving/framework/domain/*_pb2.py \
    "${TMPDIR}/adlik_serving/framework/domain"

  cp adlik_serving/clients/task/__init__.py \
    "${TMPDIR}/adlik_serving/."

  touch "${TMPDIR}/adlik_serving/apis/__init__.py"
  touch "${TMPDIR}/adlik_serving/framework/__init__.py"
  touch "${TMPDIR}/adlik_serving/framework/domain/__init__.py"


  echo "Adding package setup files"
  cp ${PIP_SRC_DIR}/setup.py "${TMPDIR}"

  pushd "${TMPDIR}"
  echo $(date) : "=== Building wheel (CPU)"
  python3 setup.py bdist_wheel --universal \
    --project_name adlik-serving-api # >/dev/null
  # echo $(date) : "=== Building wheel (GPU)"
  # python3 setup.py bdist_wheel --universal \
  #   --project_name adlik-serving-api-gpu # >/dev/null
  mkdir -p "${DEST}"
  cp dist/* "${DEST}"
  popd
  rm -rf "${TMPDIR}"
  echo $(date) : "=== Output wheel file is in: ${DEST}"
}

main "$@"
