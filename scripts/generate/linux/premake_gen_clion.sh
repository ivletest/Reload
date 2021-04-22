#!/bin/bash

#PLATFORM="$(uname)_$(uname -m)"

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../../

#mkdir -p build/$PLATFORM/Debug
#mkdir -p build/$PLATFORM/Release

tools/premake5/premake5 clion --scripts=scripts/premake