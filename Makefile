
.PHONY: all clean

all: UBOOT INITRAMFS ROOTFS

clean: UBOOT_CLEAN INITRAMFS_CLEAN ROOTFS_CLEAN DROPBEAR_CLEAN LIBNL_CLEAN IW_CLEAN


UBOOT:
	@echo "\n----------Building u-boot----------"
	\cp scripts/.uboot_config u-boot/.config
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C u-boot/ 
	cp u-boot/u-boot.bin .

UBOOT_CLEAN:
	@echo "\n----------Cleaning u-boot----------"
	-$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C u-boot/ distclean



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
	-$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C busybox-1.31.1/ distclean



ROOTFS: IW DROPBEAR
	@echo "\n---------- Creating BusyBox components needed for rootfs ----------"
	\cp scripts/.busybox_config_rootfs busybox-1.31.1/.config
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ 
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j6 -C busybox-1.31.1/ install
	@echo "\n---------- Assembling files into rootfs ----------"
	cd scripts && ./assemble_rootfs.sh

ROOTFS_CLEAN:
	@echo "\n---------- Removing all files and dirs related to assembling rootfs ----------"
	-rm -rf rootfs/*
	-rm -rf busybox-1.31.1/install_rootfs
	-$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C busybox-1.31.1 distclean



DROPBEAR:
	@echo "\n---------- Configuring and building dropbear ----------"
	cd dropbear-2019.78 && ./configure --host=armv8-rpi3-linux-gnueabihf --prefix=`pwd` --disable-zlib CC=${CROSS_COMPILE}gcc LD=${CROSS_COMPILE}ld
	$(MAKE) -C dropbear-2019.78 -j6 
	$(MAKE) -C dropbear-2019.78 install

DROPBEAR_CLEAN:
	@echo "\n---------- Cleaning dropbear folder ----------"
	-$(MAKE) -C dropbear-2019.78 distclean



LIBNL:
	@echo "\n---------- Building libnl ----------"
	cd libnl-3.2.25 && ./configure --host=armv8-rpi3-linux-gnueabihf --prefix=`pwd`/install 
	$(MAKE) -C libnl-3.2.25 -j6
	$(MAKE) -C libnl-3.2.25 install
	$(MAKE) -C libnl-3.2.25/include install

LIBNL_CLEAN:
	@echo "\n---------- Cleaning libnl ----------"
	-rm -rf libnl-3.2.25/install 
	-$(MAKE) -C libnl-3.2.25 distclean



IW: LIBNL
	@echo "\n---------- Building libnl ----------"
	export PKG_CONFIG_PATH=`pwd`/libnl-3.2.25/install/lib/pkgconfig && export CC=${CROSS_COMPILE}gcc && $(MAKE) -C iw-5.4 -j6

IW_CLEAN:
	@echo "\n---------- Cleaning libnl ----------"
	-cd iw-5.4 && rm -f iw *.o *~ *.gz version.c *-stamp nl80211-commands.inc
	# export PKG_CONFIG_PATH=`pwd`/libnl-3.2.25/install/lib/pkgconfig && export CC=${CROSS_COMPILE}gcc && $(MAKE) -C iw-5.4 clean
	# We manually rm the files rather than calling make clean because the latter requires the libnl pkgconfig to exist, which won't exist if libnl was cleaned first



OPENSSL:
	@echo "\n---------- Building Openssl ----------"
	cd openssl-1.1.1g && ./Configure ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} linux-generic32 --prefix=`pwd`/install
	$(MAKE) -C openssl-1.1.1g -j6
	$(MAKE) -C openssl-1.1.1g install

OPENSSL_CLEAN:
	@echo "\n---------- Cleaning Openssl ----------"
	-rm -rf openssl-1.1.1g/install
	-$(MAKE) -C openssl-1.1.1g distclean

