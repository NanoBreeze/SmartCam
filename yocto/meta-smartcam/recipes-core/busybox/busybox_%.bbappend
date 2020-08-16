# We want to use our own rcS rather than poky's

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# I think the recipe's SRC_URI already includes inittab, but placing it here just in case
SRC_URI_append = " \
	file://rcS \
"