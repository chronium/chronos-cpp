#!/bin/sh
set -e

cd build

ninja -v 
ninja -v install
ninja -v image
ninja -v run
