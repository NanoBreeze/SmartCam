
.PHONY: all clean

all: UBOOT INITRAMFS ROOTFS

clean: UBOOT_CLEAN INITRAMFS_CLEAN ROOTFS_CLEAN


UBOOT:
	@echo "\n----------Building u-boot----------"
	\cp scripts/.uboot_config u-boot/.config
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C u-boot/ 
	cp u-boot/u-boot.bin .


UBOOT_CLEAN:
	@echo "\n----------Cleaning u-boot----------"
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C u-boot/ clean



#Note: The patch has already been applied to the source code, we don't need to apply it again
INITRAMFS:
	@echo "\n----------Building initramfs from BusyBox ----------"
	\cp scripts/.busybox_config_initramfs busybox-1.31.1/.config
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ install
	cd scripts && ./populate_initramfs.sh
	cd busybox-1.31.1/install_initramfs && find . -print0 | cpio --null --create --verbose --format=newc | gzip --best > ../../initramfs.cpio.gz
	mkimage -A arm -O linux -T ramdisk -C gzip -d initramfs.cpio.gz initramfs.cpio.gz.uImage


INITRAMFS_CLEAN:
	@echo "\n----------Cleaning initramfs from BusyBox----------"
	-rm -rf busybox-1.31.1/install_initramfs
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C busybox-1.31.1/ clean


ROOTFS: 
	@echo "\n---------- Creating components needed for rootfs ----------"
	\cp scripts/.busybox_config_rootfs busybox-1.31.1/.config
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ 
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ install
	@echo "\n---------- Assembling files into rootfs ----------"
	cd scripts && ./assemble_rootfs.sh


ROOTFS_CLEAN:
	@echo "\n---------- Removing all files and dirs related to assembling rootfs ----------"
	-rm -rf rootfs/*
	-rm -rf busybox-1.31.1/install_rootfs
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C busybox-1.31.1/ clean