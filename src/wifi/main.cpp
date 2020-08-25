#include <stddef.h>
#include "wpa_ctrl.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::getline;
using std::istringstream;

#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant/wlan0"

namespace Command{
	const char* PING = "PING";
	const char* SCAN = "SCAN";
	const char* SCAN_RESULTS = "SCAN_RESULTS";
	const char* LIST_NETWORKS = "LIST_NETWORKS";
};

enum class ErrorCode {
	OK = 0,
	SCAN_ERROR,
	SCAN_RESULTS_ERROR,
	LIST_NETWORKS_ERROR
};

struct NetworkConf {
	unsigned int id;
	string ssid;
};

struct ScanResult {
	string bssid;
	string ssid;
};

vector<ScanResult> scan(struct wpa_ctrl* control) {
	char chScanReply[1];
	size_t chScanReplySz = sizeof(chScanReply);
	int ret = wpa_ctrl_request(control, Command::SCAN, strlen(Command::SCAN), chScanReply, &chScanReplySz, NULL);
	if (ret != 0) {
		cerr << "Unable to scan" << endl;
		throw ErrorCode::SCAN_ERROR;
	}
	
	char chScanResults[3000] = {0}; // a safer approach is to loop thru BSS <idx> to avoid problems with filtering too many scan results 
	size_t chScanResultsSize = sizeof(chScanResults);
	ret = wpa_ctrl_request(control, Command::SCAN_RESULTS, strlen(Command::SCAN_RESULTS), chScanResults, &chScanResultsSize, NULL);
	if (ret != 0) {
		cerr << "Problem with listing scan results" << endl;
		throw ErrorCode::SCAN_RESULTS_ERROR;
	}
	
	vector<ScanResult> scanResults;
	istringstream ssScanResults(chScanResults);
	string line;
	getline(ssScanResults, line); // get rid of header line
	while (getline(ssScanResults, line)) {
		// cout << line << endl;
		ScanResult scanResult;
		istringstream iss(line);
		string text;
		getline(iss, text, '\t');
		//cout << text << endl;
		scanResult.bssid = text; 
		getline(iss, text, '\t');
		getline(iss, text, '\t');
		getline(iss, text, '\t');
		getline(iss, text, '\t');

		scanResult.ssid = text; 
		//cout << text << endl;
		scanResults.push_back(scanResult);
	}
	
	//cout << scanResults.size() << endl;
	return scanResults;
}

vector<NetworkConf> list_all_networks_in_conf(struct wpa_ctrl* control) {
	char conf_networks[1000] = {0};
	size_t sz_conf_networks = sizeof(conf_networks);
	int ret = wpa_ctrl_request(control, Command::LIST_NETWORKS, strlen(Command::LIST_NETWORKS), conf_networks, &sz_conf_networks, NULL);
	if (ret != 0) {
		cerr << "Problem with listing networks in wpa_supplicant.conf" << endl;
		throw ErrorCode::LIST_NETWORKS_ERROR;
	}
	
	vector<NetworkConf> networkConfs;
	istringstream ss_conf_networks(conf_networks);
	string line;
	getline(ss_conf_networks, line); // get rid of header line
	while (getline(ss_conf_networks, line)) {
		// cout << line << endl;
		NetworkConf networkConf;
		istringstream iss(line);
		string text;
		getline(iss, text, '\t');
		//cout << text << endl;
		networkConf.id = stoi(text); 
		getline(iss, text, '\t');
		networkConf.ssid = text; 
		networkConfs.push_back(networkConf);
	}
	
	
	//cout << conf_networks << endl;
	return networkConfs;
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
	
	vector<ScanResult> scanResults = scan(control);
	cout << "scanResult.size() is: " << scanResults.size() << endl;
	
	vector<NetworkConf> networkConfs = list_all_networks_in_conf(control); 
	cout << "networkConfs.size() is: " << networkConfs.size() << endl;
	
	return 0;
}
