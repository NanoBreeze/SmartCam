
.PHONY: all clean install

all: UBOOT INITRAMFS install

clean: UBOOT_CLEAN INITRAMFS_CLEAN

install: UBOOT_INSTALL INITRAMFS_INSTALL

UBOOT:
	@echo "\n----------Building u-boot----------"
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C u-boot/ 


UBOOT_CLEAN:
	@echo "\n----------Cleaning u-boot----------"
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C u-boot/ clean


UBOOT_INSTALL:
	@echo "\n----------Installing u-boot----------"
	cp u-boot/u-boot.bin .


#Note: The patch has already been applied to the source code, we don't need to apply it again
INITRAMFS:
	@echo "\n----------Building initramfs from BusyBox ----------"
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ 

INITRAMFS_CLEAN:
	@echo "\n----------Cleaning initramfs from BusyBox----------"
	-rm -rf busybox-1.31.1/_install
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C busybox-1.31.1/ clean

INITRAMFS_INSTALL:
	@echo "\n----------Installing initramfs from BusyBox ----------"
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ install
	cp busybox-1.31.1/initramfs_init busybox-1.31.1/_install/init
	cd busybox-1.31.1/_install && find . -print0 | cpio --null --create --verbose --format=newc | gzip --best > ../../initramfs.cpio.gz
	mkimage -A arm -O linux -T ramdisk -C gzip -d initramfs.cpio.gz initramfs.cpio.gz.uImage
	
