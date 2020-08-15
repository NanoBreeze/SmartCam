FILESEXTRAPATHS_append := ":${THISDIR}/${PN}"

SRC_URI_append = " file://dropbear_banner.txt "

do_install_append() {
	install -m 0755 ${WORKDIR}/dropbear_banner.txt ${D}${sysconfdir}/
}
