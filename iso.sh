#!/bin/sh

rm -rf $OUT/iso_root
mkdir -p $OUT/iso_root
cp -r $BASE/sysroot/* $OUT/iso_root/
cp $BASE/limine/limine.sys $BASE/limine/limine-cd.bin $BASE/limine/limine-eltorito-efi.bin $OUT/iso_root/

xorriso -as mkisofs -b limine-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot limine-eltorito-efi.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    $OUT/iso_root -o $OUTPUT

$BASE/limine/limine-install $OUTPUT
rm -rf $OUT/iso_root
