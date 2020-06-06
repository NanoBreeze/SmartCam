#!/bin/sh
# Populate _install with all the relevant files

echo "Inside populate_initramfs.sh"
\cp initramfs_init _install/init
mkdir -p _install/usr/share/udhcpc
cp udhcpc_scripts/* _install/usr/share/udhcpc/ 

mkdir -p _install/etc/udhcpc
