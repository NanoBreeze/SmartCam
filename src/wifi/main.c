#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "wpa_ctrl.h"

#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant/wlan0"

int main() {
	struct wpa_ctrl* control = wpa_ctrl_open(CONFIG_CTRL_IFACE_DIR);
	const char* cmd = "PING";
	char reply[10];
	size_t szReply = 10; 
	int ret = wpa_ctrl_request(control, cmd, strlen(cmd), reply, &szReply, NULL);
	if (ret == 0) {
		printf("success. The reply is: %s", reply);
	}
	else if (ret == -1)
		printf("error\n");
	else 
		printf("timeout\n");

	return 0;
}
