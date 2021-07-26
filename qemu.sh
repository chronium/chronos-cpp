#!/bin/sh

qemu-system-$($BASE/target-triplet-to-arch.sh $HOST) \
    -M q35 \
    -m 2G \
    -cdrom $IMAGE \
    -no-reboot \