#include <common.h>
#include <command.h>

static int show_hint(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    printf("kernel7 is the original Raspbian image\n"
	   "For some reason, to use the initramfs, we must first load it, then the kernel, then the flattened device tree. Eg) setenv serverip 10.88.111.18; run loadInitramfs; run loadKernel; run loadFdt; run loadEnv2; run doBoot2\n"
            "This is the bootcommand for tftp. It uses hardcoded server ip. If using fatload, make some small adjustments:\n"
            "dhcp; tftp ${kernel_addr_r} 10.88.111.16:kernel.img; tftp 0x2000000 10.88.111.16:bcm2710-rpi-3-b.dtb; echo ------TFTP complete------; setenv bootargs 8250.nr_uarts=1 root=/dev/mmcblk0p2 rootwait console=ttyS0,115200; bootz ${kernel_addr_r} - 0x2000000\n"
            "Have a good day!\n"
    );

	return 0;
}

U_BOOT_CMD(
	hint, 1,	0,	show_hint,
	"Shows helpful stuff",
    ""
);
