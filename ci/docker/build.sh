#!/bin/sh -e

cd "$(dirname "$0")"

docker build -t adlik-ci:latest .
