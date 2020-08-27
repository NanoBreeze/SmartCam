/* This file could do with some refactoring, especially by extracting many wpa_ctrl_request 
 * calls into a generic function. 
 * We can also attach to intercept/read control event messages
 */

#include <stddef.h>
#include "wpa_ctrl.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <algorithm>


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::getline;
using std::istringstream;
using std::begin;
using std::end;
using std::to_string;

#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant/wlan0"

namespace Command{
	const char* PING = "PING";
	const char* SCAN = "SCAN";
	const char* SCAN_RESULTS = "SCAN_RESULTS";
	const char* LIST_NETWORKS = "LIST_NETWORKS";
	const string REMOVE_NETWORK(int id) {
		return string("REMOVE_NETWORK ") + to_string(id);
	}
	const char* SAVE_CONFIG = "SAVE_CONFIG";
	const char* RECONFIGURE = "RECONFIGURE";
	const string SET_NETWORK_PASSWORD(int id, string password) {
		return string("SET_NETWORK ") + to_string(id) + " password " + "\"" + password + "\""; 
	}
	const string SET_NETWORK_PSK(int id, string psk) {
		return string("SET_NETWORK ") + to_string(id) + " psk " + "\"" + psk + "\""; 
	}
	const string SET_NETWORK_SSID(int id, string ssid) {
		return string("SET_NETWORK ") + to_string(id) + " ssid " + "\"" + ssid + "\"";
	}
	const char* DISCONNECT = "DISCONNECT";
	const char* REASSOCIATE = "REASSOCIATE";
	const char* ADD_NETWORK = "ADD_NETWORK";
	const string SELECT_NETWORK(int id) {
		return string("SELECT_NETWORK ") + to_string(id);
	}
	const string ENABLE_NETWORK(int id) {
		return string("ENABLE_NETWORK ") + to_string(id);
	}
};

enum class ErrorCode {
	OK = 0,
	SCAN_ERROR,
	SCAN_RESULTS_ERROR,
	LIST_NETWORKS_ERROR,
	REMOVE_NETWORK,
	SAVE_CONFIG,
	RECONFIGURE,
	SET_NETWORK_PASSWORD,
	DISCONNECT,
	REASSOCIATE,
	SELECT_NETWORK,
	SET_NETWORK_SSID,
	ADD_NETWORK,
	ENABLE_NETWORK
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

vector<NetworkConf> listAllNetworksInConf(struct wpa_ctrl* control) {
	/* We return a vector b/c there can be duplicant networks in the conf 
	 */
	 
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

bool changePassword(struct wpa_ctrl* control, string ssid, string password) {
	/* The ssid must exist. If it doesn't, then no new network will
	 * be added, and we shall return false
	 */
	vector<NetworkConf> networkConfs = listAllNetworksInConf(control);
	auto it = std::find_if(begin(networkConfs), end(networkConfs), [&ssid](const NetworkConf& m) {
		return m.ssid == ssid;
	});
	
	if (it == end(networkConfs))
		return false;
		
	char reply[1] = {0};
	size_t replySz = sizeof(reply);
	int ret = wpa_ctrl_request(control, Command::SET_NETWORK_PASSWORD(it->id, password).c_str(), Command::SET_NETWORK_PASSWORD(it->id, password).size(), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with changing password: " << it->ssid << endl;
		throw ErrorCode::SET_NETWORK_PASSWORD;
	 }
	 
	 ret = wpa_ctrl_request(control, Command::SET_NETWORK_PSK(it->id, password).c_str(), Command::SET_NETWORK_PSK(it->id, password).size(), reply, &replySz, NULL);
	 if (ret != 0) {
		cerr << "Problem with setting ssid: " << ssid << endl;
		throw ErrorCode::SET_NETWORK_PASSWORD;
	 }
	
	ret = wpa_ctrl_request(control, Command::SAVE_CONFIG, strlen(Command::SAVE_CONFIG), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with saving config" << endl;
		throw ErrorCode::SAVE_CONFIG;
	}
		
	return true;
}

void connect(struct wpa_ctrl* control, string ssid, string password) {
	/* We first check if the network is already listed in the conf
	 * If it is, change its password with the new password
	 * Then connect
	 */
	 if (changePassword(control, ssid, password)) {
		return;
	 }
	 
	 char reply[10] = {0};
	 size_t replySz = sizeof(reply);
	 int ret = wpa_ctrl_request(control, Command::ADD_NETWORK, strlen(Command::ADD_NETWORK), reply, &replySz, NULL);
	 if (ret != 0) {
		cerr << "Problem with changing password: " << ssid << endl;
		throw ErrorCode::ADD_NETWORK;
	 }
	 
	 int networkId = atoi(reply);
	 cout << "The networkId is: " << networkId << endl;
	 	
	 ret = wpa_ctrl_request(control, Command::SET_NETWORK_SSID(networkId, ssid).c_str(), Command::SET_NETWORK_SSID(networkId, ssid).size(), reply, &replySz, NULL);
	 if (ret != 0) {
		cerr << "Problem with setting ssid: " << ssid << endl;
		throw ErrorCode::SET_NETWORK_SSID;
	 }

	 changePassword(control, ssid, password);
	 
	 ret = wpa_ctrl_request(control, Command::ENABLE_NETWORK(networkId).c_str(), Command::ENABLE_NETWORK(networkId).size(), reply, &replySz, NULL);
	 if (ret != 0) {
		cerr << "Problem with enabling network config" << endl;
		throw ErrorCode::ENABLE_NETWORK;
	 }
	 
	 	 
	 ret = wpa_ctrl_request(control, Command::SAVE_CONFIG, strlen(Command::SAVE_CONFIG), reply, &replySz, NULL);
	 if (ret != 0) {
		cerr << "Problem with saving config" << endl;
		throw ErrorCode::SAVE_CONFIG;
	 }
}

bool connect(struct wpa_ctrl* control, string ssid) {
	// Return false if the ssid doesn't exist
	vector<NetworkConf> networkConfs = listAllNetworksInConf(control);
	auto it = std::find_if(begin(networkConfs), end(networkConfs), [&ssid](const NetworkConf& m) {
		return m.ssid == ssid;
	});
	
	if (it == end(networkConfs))
		return false;
		
	char reply[1] = {0};
	size_t replySz = sizeof(reply);
	
	int ret = wpa_ctrl_request(control, Command::SELECT_NETWORK(it->id).c_str(), Command::SELECT_NETWORK(it->id).size(), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with selecting network " << it->id << endl;;
		throw ErrorCode::SELECT_NETWORK;
	}
	
	ret = wpa_ctrl_request(control, Command::REASSOCIATE, strlen(Command::REASSOCIATE), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with reassociate" << endl;;
		throw ErrorCode::REASSOCIATE;
	}
	return true;
}

void disassociate(struct wpa_ctrl* control) {
	char reply[1] = {0};
	size_t replySz = sizeof(reply);
	int ret = wpa_ctrl_request(control, Command::DISCONNECT, strlen(Command::DISCONNECT), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with disconnecting" << endl;;
		throw ErrorCode::DISCONNECT;
	}
}

void reloadConf(struct wpa_ctrl* control) {
	char reply[1] = {0};
	size_t replySz = sizeof(reply);
	int ret = wpa_ctrl_request(control, Command::RECONFIGURE, strlen(Command::RECONFIGURE), reply, &replySz, NULL);
	if (ret != 0) {
		cerr << "Problem with reconfiguring" << endl;
		throw ErrorCode::RECONFIGURE;
	}
}

bool forgetNetwork(struct wpa_ctrl* control, string ssid) {
	vector<NetworkConf> networkConfs = listAllNetworksInConf(control);
	
	auto it = std::find_if(begin(networkConfs), end(networkConfs), [&ssid](const NetworkConf& m) {
		return m.ssid == ssid;
	});
	
	if (it != end(networkConfs)) {
		// cout << it->ssid << ": " << it->id << endl;
		char reply[1] = {0};
		size_t replySz = sizeof(reply);
		int ret = wpa_ctrl_request(control, Command::REMOVE_NETWORK(it->id).c_str(), Command::REMOVE_NETWORK(it->id).size(), reply, &replySz, NULL);
		if (ret != 0) {
			cerr << "Problem with removing network: " << it->ssid << endl;
			throw ErrorCode::REMOVE_NETWORK;
		}
		
		ret = wpa_ctrl_request(control, Command::SAVE_CONFIG, strlen(Command::SAVE_CONFIG), reply, &replySz, NULL);
		if (ret != 0) {
			cerr << "Problem with saving config" << endl;
			throw ErrorCode::SAVE_CONFIG;
		}
		
		return true;
	}
	
	return false;	
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
		printf("Success. szReply is: %d; reply is: %s\n", szReply, reply);
	}
	else if (ret == -1)
		printf("error\n");
	else
		printf("timeout\n");
	
	/*
	vector<ScanResult> scanResults = scan(control);
	cout << "scanResult.size() is: " << scanResults.size() << endl;
	
	vector<NetworkConf> networkConfs = listAllNetworksInConf(control); 
	cout << "networkConfs.size() is: " << networkConfs.size() << endl;
	*/
	reloadConf(control);
	connect(control, "bloop", "blah");
	//cout << here << endl;
	
	
	return 0;
}
