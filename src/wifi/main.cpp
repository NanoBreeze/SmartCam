#include <stddef.h>
#include "wpa_ctrl.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

using std::cout;
using std::cerr;
using std::endl;

#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant/wlan0"

namespace Command{
	const char* PING = "PING";
	const char* SCAN = "SCAN";
	const char* SCAN_RESULTS = "SCAN_RESULTS";
};

enum class ErrorCode {
	OK = 0,
	SCAN_ERROR,
	SCAN_RESULTS_ERROR
};

ErrorCode scan(struct wpa_ctrl* control) {
	char scan_reply[1];
	size_t sz_scan_reply = sizeof(scan_reply);
	int ret = wpa_ctrl_request(control, Command::SCAN, strlen(Command::SCAN), scan_reply, &sz_scan_reply, NULL);
	if (ret != 0) {
		cerr << "Unable to scan" << endl;
		return ErrorCode::SCAN_ERROR;
	}
	
	char scan_results[3000]; // a safer approach is to loop thru BSS <idx> to avoid problems with filtering too many scan results 
	size_t sz_scan_results = sizeof(scan_results);
	ret = wpa_ctrl_request(control, Command::SCAN_RESULTS, strlen(Command::SCAN_RESULTS), scan_results, &sz_scan_results, NULL);
	if (ret != 0) {
		cerr << "Problem with listing scan results" << endl;
		return ErrorCode::SCAN_RESULTS_ERROR;
	}
	
	printf("%s\n", scan_results);
	return ErrorCode::OK;
}


int main() {
	cout << "Hello there!" << endl;
	struct wpa_ctrl* control = wpa_ctrl_open(CONFIG_CTRL_IFACE_DIR);
	
	if (control == NULL) {
		cout << "The control is NULL. Exiting" << endl;
		return 0;
	}
	
	const char* cmd = "PING";
	char reply[50];
	size_t szReply = 6;
	int ret = wpa_ctrl_request(control, Command::PING, strlen(Command::PING), reply, &szReply, NULL);
	if (ret == 0) {
		printf("Success. szReply is: %d; reply is: %s", szReply, reply);
	}
	else if (ret == -1)
		printf("error\n");
	else
		printf("timeout\n");
	
	if (scan(control) != ErrorCode::OK) {
		cerr << "Failure. Exiting" << endl;
		exit(0);
	}
		
	
	return 0;
}
