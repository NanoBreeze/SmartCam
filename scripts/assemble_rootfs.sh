#!/bin/sh
# Place the relevant files into rootfs/

ROOTFS=../rootfs/

mkdir -p ${ROOTFS}/dev ${ROOTFS}/proc ${ROOTFS}/sys ${ROOTFS}/boot ${ROOTFS}/etc ${ROOTFS}/usr ${ROOTFS}/lib ${ROOTFS}/tmp ${ROOTFS}/var || true
mkdir -p ${ROOTFS}/usr/share || true

cp -r ../busybox-1.31.1/install_rootfs/* ${ROOTFS}
rsync -a --ignore-existing ../tools/armv8-rpi3-linux-gnueabihf/armv8-rpi3-linux-gnueabihf/sysroot/ ${ROOTFS} #I suppose we could also use cp, but I was curious how to use rsync --ignore-existing may not actually be needed
cp ../tools/armv8-rpi3-linux-gnueabihf/armv8-rpi3-linux-gnueabihf/debug-root/usr/bin/* ${ROOTFS}/bin

cp -r ../scripts/init.d/ ${ROOTFS}/etc/ || true
cp -r ../scripts/udhcpc_scripts/ ${ROOTFS}/usr/share/udhcpc/ || true

cp ../scripts/inittab ${ROOTFS}/etc || true

# Copy dropbear bins
cp -r ../dropbear-2019.78/sbin/* ${ROOTFS}/sbin/
cp -r ../dropbear-2019.78/bin/* ${ROOTFS}/bin/
cp -r ../scripts/dropbear/ ${ROOTFS}/etc/

# Copy pre-compiled modules and firmware stuff
cp -r ../bin/kernel_5.4.40_precompiled_modules/ ${ROOTFS}/lib/modules/
cp -r ../bin/precompiled_firmware/ ${ROOTFS}/lib/firmware/
cp -r ../bin/crda/ ${ROOTFS}/lib/

# Copy libnl and iw
cp -r ../libnl-3.2.25/install/sbin/* ${ROOTFS}/sbin/
cp -r ../libnl-3.2.25/install/lib/* ${ROOTFS}/lib/
cp ../iw-5.4/iw ${ROOTFS}/sbin/

# Copy Openssl files
cp ../openssl-1.1.1g/install/bin/* ${ROOTFS}/bin/
cp -r ../openssl-1.1.1g/install/lib/* ${ROOTFS}/lib/
cp -r ../openssl-1.1.1g/install/ssl/ ${ROOTFS}/etc/

# Copy wpa_supplicant, wpa_passphrase, wpa_cli, and wpa_supplicant.conf, which contains hashed pre-shared key for whitebyer_2.4ghz 
cp ../wpa_supplicant-2.9/wpa_supplicant/wpa_cli ${ROOTFS}/bin
cp ../wpa_supplicant-2.9/wpa_supplicant/wpa_passphrase ${ROOTFS}/bin
cp ../wpa_supplicant-2.9/wpa_supplicant/wpa_supplicant ${ROOTFS}/bin
cp -r ../scripts/wpa_supplicant/ ${ROOTFS}/etc/