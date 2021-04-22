#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../../

mkdir -p build/"$(uname)"/Debug
mkdir -p build/"$(uname)"/Release

tools/premake5/premake5 clion --scripts=scripts/premake

cp -r engine/assets/* build/"$(uname)"/Debug
cp -r engine/assets/* build/"$(uname)"/Release