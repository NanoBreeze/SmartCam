# We want to use our custom inittab rather than BusyBox's default one (actually, I think they might be the same
# but to play it safe, we'll use our own from the Make-based distro, which we know would work

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

do_determinePN() {
	bbwarn "The value of PN is: ${PN}"
}

# I think the recipe's SRC_URI already includes inittab, but placing it here just in case
SRC_URI_append = " \
	file://inittab \
"

addtask determinePN