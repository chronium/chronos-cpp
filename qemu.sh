#!/bin/bash

qemu-system-$($BASE/target-triplet-to-arch.sh $HOST) \
    -M q35 \
    -m 2G \
    -cdrom $IMAGE \
    -no-reboot \
    -serial stdio \
    -rtc base=localtime,clock=host,driftfix=slew