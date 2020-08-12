#require recipes-core/images/core-image-minimal.bb

LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL_append = " \
    openssl \
    dropbear \
    wpa-supplicant \
    busybox \
    iw \
    lrzsz \
    python3 "