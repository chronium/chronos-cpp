#!/bin/bash
set -e

rm -rf build
rm -rf sysroot

cmake -B./build -G Ninja -DCMAKE_TOOLCHAIN_FILE=/home/chronium/chronos/cmake/toolchain-amd64-clang.cmake

cd build

ninja 
ninja install
ninja image
ninja qemu
