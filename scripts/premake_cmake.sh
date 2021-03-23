#!/bin/bash

set -e

BUILD_TYPE=${BUILD_TYPE:-Debug}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../

tools/premake5 cmake
cd build
cmake .
make "$*"
