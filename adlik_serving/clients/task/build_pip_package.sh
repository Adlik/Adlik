#!/bin/bash

set -e

function main() {
  if [[ $# -lt 1 ]] ; then
    echo "No destination dir provided."
    echo "Example usage: $0 /tmp/pip"
    exit 1
  fi

  local BAZEL_PROJECT_DIR="bazel-${PWD##*/}"
  local PIP_BIN_DIR=$(dirname $0)
  local ADLIK_BIN_ROOT_DIR=${PIP_BIN_DIR}/../../..
  DEST="$1"
  TMPDIR="$(mktemp -d)"
  local PIP_SRC_DIR=${PIP_BIN_DIR}/build_pip_package.runfiles/Adlik/adlik_serving/clients/task
  if [[ ! -d ${PIP_SRC_DIR} ]]; then
    local PIP_SRC_DIR=${PIP_BIN_DIR}/build_pip_package.runfiles/adlik/adlik_serving/clients/task
  fi

  echo $(date) : "=== Using tmpdir: ${TMPDIR}"
  mkdir -p ${TMPDIR}/adlik_serving/framework/domain
  mkdir -p ${TMPDIR}/adlik_serving/apis

  echo "Adding python files"
  cp ${ADLIK_BIN_ROOT_DIR}/adlik_serving/apis/*_pb2.py \
    "${TMPDIR}/adlik_serving/apis"

  cp ${ADLIK_BIN_ROOT_DIR}/adlik_serving/apis/*_pb2_grpc.py \
    "${TMPDIR}/adlik_serving/apis"

  cp ${ADLIK_BIN_ROOT_DIR}/adlik_serving/framework/domain/*_pb2.py \
    "${TMPDIR}/adlik_serving/framework/domain"

  # cp adlik_serving/clients/task/__init__.py \
  #   "${TMPDIR}/adlik_serving/."

  touch "${TMPDIR}/adlik_serving/__init__.py"
  touch "${TMPDIR}/adlik_serving/apis/__init__.py"
  touch "${TMPDIR}/adlik_serving/framework/__init__.py"
  touch "${TMPDIR}/adlik_serving/framework/domain/__init__.py"

  echo "Adding package setup files"
  cp ${PIP_SRC_DIR}/setup.py "${TMPDIR}"

  pushd "${TMPDIR}"
  echo $(date) : "=== Building wheel (CPU)"
  python3 setup.py bdist_wheel --universal \
    --project_name adlik-serving-api # >/dev/null

  mkdir -p "${DEST}"
  cp dist/* "${DEST}"
  popd
  rm -rf "${TMPDIR}"
  echo $(date) : "=== Output wheel file is in: ${DEST}"
}

main "$@"
