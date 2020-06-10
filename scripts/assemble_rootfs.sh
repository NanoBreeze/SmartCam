#!/bin/sh
# Place the relevant files into rootfs/

ROOTFS=../rootfs/

cp -r ../busybox-1.31.1/install_rootfs/* ${ROOTFS}
rsync -a --ignore-existing ../tools/armv8-rpi3-linux-gnueabihf/armv8-rpi3-linux-gnueabihf/sysroot/ ${ROOTFS} #I suppose we could also use cp, but I was curious how to use rsync --ignore-existing may not actually be needed

mkdir -p ${ROOTFS}/dev ${ROOTFS}/proc ${ROOTFS}/sys ${ROOTFS}/boot ${ROOTFS}/etc ${ROOTFS}/usr || true
mkdir -p ${ROOTFS}/usr/share || true

cp -r ../scripts/init.d ${ROOTFS}/etc/init.d || true
cp -r ../scripts/udhcpc_scripts/ ${ROOTFS}/usr/share/udhcpc/ || true

cp ../scripts/inittab ${ROOTFS}/etc || true

