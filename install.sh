#!/usr/bin/env bash

set -xe

SUDO_VENDOR="sudo"
BUILD_DIR="./build/Desktop-Debug/"
PREFIX="/usr/bin"

$SUDO_VENDOR install -m775 $BUILD_DIR/details $PREFIX/details
