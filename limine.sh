#!/bin/sh
set -e

git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1

cd limine
rm -rf .git
make