#!/bin/sh
# Populate the folder containing the initramfs with all the relevant files

INITRAMFS_INSTALL_DIR=../busybox-1.31.1/install_initramfs/

echo "Inside populate_initramfs.sh"
\cp initramfs_init ${INITRAMFS_INSTALL_DIR}
mkdir -p ${INITRAMFS_INSTALL_DIR}/usr/share/udhcpc
\cp udhcpc_scripts/* ${INITRAMFS_INSTALL_DIR}/usr/share/udhcpc/ 
