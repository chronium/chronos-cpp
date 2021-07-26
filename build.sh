#!/bin/sh
export HOST=x86_64-pc-unknown-elf

set -e

pushd build

ninja
ninja install
ninja image

popd
