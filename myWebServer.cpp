/*
myWebServer.cpp for esp8266

Copyright (c) 2016 David Paiva (david@nailbuster.com). All rights reserved.


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "myWebServer.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h> 
#include "htmlEmbed.h"
#include <TimeLib.h>


ESP8266WebServer server(80);
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
bool dsnServerActive = false;
String ConfigUsername="admin";   //used for webconfig username admin
String ConfigPassword="";        //used for webconfig username admin
File fsUploadFile;


//ntp stuffs
WiFiUDP UDPNTPClient;
unsigned long lastTimeCheck = 0;  //for timer to fire every sec
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
time_t prevDisplay = 0; // when the digital clock was displayed

MyWebServerClass MyWebServer;
String MyWebServerLog;



// send an NTP request to the time server at the given address
void ICACHE_FLASH_ATTR sendNTPpacket(IPAddress &address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
							 // 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	UDPNTPClient.beginPacket(address, 123); //NTP requests are to port 123
	UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
	UDPNTPClient.endPacket();
}


time_t ICACHE_FLASH_ATTR getNtpTime()
{
	
	while (UDPNTPClient.parsePacket() > 0); // discard any previously received packets
	DebugPrintln("Transmit NTP Request");
	IPAddress timeServerIP;
	WiFi.hostByName(MyWebServer.ntpServerName.c_str(), timeServerIP);
	sendNTPpacket(timeServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = UDPNTPClient.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			DebugPrintln("Receive NTP Response");
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + (MyWebServer.timezone/10) * SECS_PER_HOUR;
		}
	}
	DebugPrintln("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}





// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char ICACHE_FLASH_ATTR h2int(char c)
{
	if (c >= '0' && c <= '9') {
		return((unsigned char)c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return((unsigned char)c - 'a' + 10);
	}
	if (c >= 'A' && c <= 'F') {
		return((unsigned char)c - 'A' + 10);
	}
	return(0);
}

String ICACHE_FLASH_ATTR MyWebServerClass::urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
	char c;
	String ret = "";

	for (byte t = 0; t<input.length(); t++)
	{
		c = input[t];
		if (c == '+') c = ' ';
		if (c == '%') {


			t++;
			c = input[t];
			t++;
			c = (h2int(c) << 4) | h2int(input[t]);
		}

		ret.concat(c);
	}
	return ret;

}


String ICACHE_FLASH_ATTR MyWebServerClass::urlencode(String str)
{
	String encodedString = "";
	char c;
	char code0;
	char code1;
	char code2;
	for (int i = 0; i < str.length(); i++) {
		c = str.charAt(i);
		if (c == ' ') {
			encodedString += '+';
		}
		else if (isalnum(c)) {
			encodedString += c;
		}
		else {
			code1 = (c & 0xf) + '0';
			if ((c & 0xf) >9) {
				code1 = (c & 0xf) - 10 + 'A';
			}
			c = (c >> 4) & 0xf;
			code0 = c + '0';
			if (c > 9) {
				code0 = c - 10 + 'A';
			}
			code2 = '\0';
			encodedString += '%';
			encodedString += code0;
			encodedString += code1;
			//encodedString+=code2;
		}
		yield();
	}
	return encodedString;

}






bool isAdmin()
{
	if (ConfigPassword == "") return true; //not using web password (default);
	
	bool isAuth = false;
	if (!server.authenticate(ConfigUsername.c_str(), ConfigPassword.c_str()))
	{
		server.requestAuthentication();

	}
	else isAuth = true;

	return isAuth;  
}

String getContentType(String filename)
{
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}




bool handleFileRead(String path)
{
	DebugPrintln("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.html";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (isAdmin())
	{
		if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
			if (SPIFFS.exists(pathWithGz))
				path += ".gz";
			File file = SPIFFS.open(path, "r");
			size_t sent = server.streamFile(file, contentType);
			file.close();
			return true;
		}
	}
	return false;
}

void handleFileUpload()
{
	if (server.uri() != "/upload") return;
	if (isAdmin() == false) return;
	MyWebServer.isDownloading = true;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
		DebugPrintln("handleFileUpload Name: "); DebugPrintln(filename);
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		//SerialLog.print("handleFileUpload Data: "); DebugPrintln(upload.currentSize);
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile)
			fsUploadFile.close();
		
		DebugPrint("handleFileUpload Size: "); DebugPrintln(upload.totalSize);
//		MyWebServer.isDownloading = false;
	}
}

void handleFileDelete(String fname)
{
	if (isAdmin() == false) return;
	DebugPrintln("handleFileDelete: " + fname);
	fname = '/' + fname;
	if (!SPIFFS.exists(fname))
		return server.send(404, "text/plain", "FileNotFound");
	if (SPIFFS.exists(fname))
	{
		SPIFFS.remove(fname);
		server.send(200, "text/plain", "");
	}
}

void handleJsonSave()
{
	if (isAdmin() == false) return;
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD JsonSave ARGS");

	String fname = "/" + server.arg(0);

	DebugPrintln("handleJsonSave: " + fname);

	File file = SPIFFS.open(fname, "w");
	if (file) {
		file.println(server.arg(1));  //save json data
		file.close();
	}
	else  //cant create file
		return server.send(500, "text/plain", "JSONSave FAILED");
	server.send(200, "text/plain", "");

	if (MyWebServer.jsonSaveHandle != NULL)	MyWebServer.jsonSaveHandle(fname);
}

void handleJsonLoad()
{
	if (isAdmin() == false) return;
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD JsonLoad ARGS");
	String fname = "/" + server.arg(0);
	DebugPrintln("handleJsonRead: " + fname);

	File file = SPIFFS.open(fname, "r");
	if (file) {
		server.streamFile(file, "application/json");
		file.close();
	}
}

bool handleFileDownload(String fname)
{
	if (isAdmin() == false) return false;
	DebugPrintln("handleFileDownload: " + fname);
	String contentType = "application/octet-stream";
	fname = "/" + fname;
	if (SPIFFS.exists(fname)) {
		File file = SPIFFS.open(fname, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void handleFileList()
{
	if (isAdmin() == false) return;
	Dir dir = SPIFFS.openDir("/");
	String output = "{\"success\":true, \"is_writable\" : true, \"results\" :[";
	bool firstrec = true;
	while (dir.next()) {
		if (!firstrec) { output += ','; }  //add comma between recs (not first rec)
		else {
			firstrec = false;
		}
		String fn = dir.fileName();
		fn.remove(0, 1); //remove slash
		output += "{\"is_dir\":false";
		output += ",\"name\":\"" + fn;
		output += "\",\"size\":" + String(dir.fileSize());
		output += ",\"path\":\"";
		output += "\",\"is_deleteable\":true";
		output += ",\"is_readable\":true";
		output += ",\"is_writable\":true";
		output += ",\"is_executable\":true";
		output += ",\"mtime\":1452813740";   //dummy time
		output += "}";
	}
	output += "]}";
	//DebugPrintln("got list >"+output);
	server.send(200, "text/json", output);
}

void HandleFileBrowser()
{
	if (isAdmin() == false) return;
	if (server.arg("do") == "list") {
		handleFileList();
	}
	else
		if (server.arg("do") == "delete") {
			handleFileDelete(server.arg("file"));
		}
		else
			if (server.arg("do") == "download") {
				handleFileDownload(server.arg("file"));
			}
			else
			{
				if (!handleFileRead("/filebrowse.html")) { //send GZ version of embedded browser
																server.sendHeader("Content-Encoding", "gzip");
																server.send_P(200, "text/html", PAGE_FSBROWSE, sizeof(PAGE_FSBROWSE));
														 }
				MyWebServer.isDownloading = true; //need to stop all cloud services from doing anything!  crashes on upload with mqtt...
			}
}

void formatspiffs()
{
	if (isAdmin() == false) return;
	DebugPrintln("formatting spiff...");
	if (!SPIFFS.format()) {
		DebugPrintln("Format failed");
	}
	else { DebugPrintln("format done...."); }
	server.send(200, "text/html", "Format Finished....rebooting");
}

void handleESPUpdate(){
	if (isAdmin() == false) return;
	// handler for the file upload, get's the sketch bytes, and writes
	// them through the Update object
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		WiFiUDP::stopAll();
		MyWebServer.OTAisflashing = true;  
		DebugPrintln("Update: " + upload.filename);
		uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		if (!Update.begin(maxSketchSpace)) {//start with max available size
			Update.printError(Serial);
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		DebugPrint(".");
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
			Update.printError(Serial);

		}
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (Update.end(true)) { //true to set the size to the current progress
			DebugPrintln("Update Success: \nRebooting...\n"+ String(upload.totalSize));
		}
		else {
			Update.printError(Serial);
		}		
	}
	else if (upload.status == UPLOAD_FILE_ABORTED) {
		Update.end();
		DebugPrintln("Update was aborted");
		MyWebServer.OTAisflashing = false;
	}
	delay(0);
};


void FileSaveContent_P(String fname, PGM_P content, u_long numbytes, bool overWrite = false) {   //save PROGMEM array to spiffs file....//f must be already open for write!

	if (SPIFFS.exists(fname) & overWrite == false) return;


	const int writepagesize = 1024;
	char contentUnit[writepagesize + 1];
	contentUnit[writepagesize] = '\0';
	u_long remaining_size = numbytes;


	File f = SPIFFS.open(fname, "w");



	if (f) { // we could open the file 

		while (content != NULL && remaining_size > 0) {
			size_t contentUnitLen = writepagesize;

			if (remaining_size < writepagesize) contentUnitLen = remaining_size;
			// due to the memcpy signature, lots of casts are needed
			memcpy_P((void*)contentUnit, (PGM_VOID_P)content, contentUnitLen);

			content += contentUnitLen;
			remaining_size -= contentUnitLen;

			// write is so overloaded, had to use the cast to get it pick the right one
			f.write((uint8_t *)contentUnit, contentUnitLen);
		}
		f.close();
		DebugPrintln("created:" + fname);
	}
}




void CheckNewSystem() {   //if new system we save the embedded htmls into the root of Spiffs as .gz!
	
	FileSaveContent_P("/wifisetup.html.gz", PAGE_WIFISETUP, sizeof(PAGE_WIFISETUP), false);
	FileSaveContent_P("/filebrowse.html.gz", PAGE_FSBROWSE, sizeof(PAGE_FSBROWSE), false);
}



void ICACHE_FLASH_ATTR restartESP() {
	if (isAdmin() == false) return;
	server.send(200, "text/plain", "Restarting ESP...");
	delay(100);
	ESP.restart();
}

void ICACHE_FLASH_ATTR sendNetworkStatus()
{
	uint8_t mac[6];
	char macStr[18] = { 0 };
	WiFi.macAddress(mac);
	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";

	int n = WiFi.scanNetworks();

	if (n == 0)
	{
		Networks = "<font color='#FF0000'>No networks found!</font>";
	}
	else
	{
		Networks = "Found " + String(n) + " Networks<br>";
		Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i)
		{
			int quality = 0;
			if (WiFi.RSSI(i) <= -100)
			{
				quality = 0;
			}
			else if (WiFi.RSSI(i) >= -50)
			{
				quality = 100;
			}
			else
			{
				quality = 2 * (WiFi.RSSI(i) + 100);
			}
			Networks += "<tr><td>" + String(WiFi.SSID(i)) + "</td><td>" + String(quality) + "%</td><td>" + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
		}
		Networks += "</table>";
	}

	String wifilist = "";
	wifilist += "WiFi State: " + state + "<br>";
	wifilist += "Scanned Networks <br>" + Networks + "<br>";

	String values = "";
	values += "<body> SSID          :" + (String)WiFi.SSID() + "<br>";
	values += "IP Address     :   " + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "<br>";
	values += "Wifi Gateway   :   " + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "<br>";
	values += "NetMask        :   " + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "<br>";
	values += "Mac Address    >   " + String(macStr) + "<br>";
	values += "NTP Time       :   " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(year()) + "-" + String(month()) + "-" + String(day()) + "<br>";
	values += "Server Uptime  :   " + String(millis()/60000) + " minutes"+ "<br>";
	values += wifilist;
	values += " <input action=\"action\" type=\"button\" value=\"Back\" onclick=\"history.go(-1);\" style=\"width: 100px; height: 50px;\" /> </body> ";
	server.send(200, "text/html", values);
}

void handleWifiConfig() {
	//send GZ version of embedded config
	server.sendHeader("Content-Encoding", "gzip");
	server.send_P(200, "text/html", PAGE_WIFISETUP, sizeof(PAGE_WIFISETUP));
}

void handleRoot() {  //handles root of website (used in case of virgin systems.)

	if (!handleFileRead("/")) {   //if new system without index we either show wifisetup or if already setup/connected we show filebrowser for config.
		if (isAdmin()) {
			if (WiFi.status() != WL_CONNECTED) {
				handleWifiConfig();
			}
			else { HandleFileBrowser(); }
		}
	}
 //use indexhtml or use embedded wifi setup...	


}

String MyWebServerClass::CurTimeString() {
//	return String(hour()) + ":" + String(minute()) + ":" + String(second());
	char tmpStr[20];
	sprintf(tmpStr, "%02d:%02d:%02d %s", hourFormat12(), minute(), second(), (isAM() ? "AM" : "PM"));
	return String(tmpStr);
}
String MyWebServerClass::CurDateString() {
	return String(year()) + "-" + String(month()) + "-" + String(day());
}

void MyWebServerClass::begin()
{
//SERVER STARTUP
//SERVER INIT


	DebugPrintln("Starting ES8266");

	bool result = SPIFFS.begin();
	DebugPrintln("SPIFFS opened: " + result);

	if (!WiFiLoadconfig())   //read network ..
	{
		// DEFAULT CONFIG
		ssid = "MYSSID";
		password = "MYPASSWORD";
		dhcp = true;
		IP[0] = 192; IP[1] = 168; IP[2] = 1; IP[3] = 100;
		Netmask[0] = 255; Netmask[1] = 255; Netmask[2] = 255; Netmask[3] = 0;
		Gateway[0] = 192; Gateway[1] = 168; Gateway[2] = 1; Gateway[3] = 1;
		ntpServerName = "0.de.pool.ntp.org";
		UpdateNTPEvery = 0;
		timezone = -10;
		daylight = true;
		DeviceName = "myESP";
		useNTP = false;
		ConfigPassword = "";
		DebugPrintln("General config applied");
	}

	String chipid = String(ESP.getChipId(), HEX);
	String hostname = "myESP" + chipid;
	

	WiFi.hostname(DeviceName);

	WiFi.mode(WIFI_STA);
	delay(10);

	//try to connect
	
	WiFi.begin(ssid.c_str(), password.c_str());


	DebugPrintln("Wait for WiFi connection.");

	// ... Give ESP 20 seconds to connect to station.
	unsigned long startTime = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000 &&  !(ssid=="MYSSID"))
	{
		DebugPrint('.');
		// SerialLog.print(WiFi.status());
		delay(500);
	}


	if (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		DebugPrintln("cannot connect to: " + ssid);
	}
	else if (!dhcp)	{
		WiFi.config(IPAddress(IP[0], IP[1], IP[2], IP[3]), IPAddress(Gateway[0], Gateway[1], Gateway[2], Gateway[3]), IPAddress(Netmask[0], Netmask[1], Netmask[2], Netmask[3]));
	}




	bool StartAP = false;
	// Check connection
	if (WiFi.status() == WL_CONNECTED)
	{
		// ... print IP Address
		DebugPrint("IP address: ");
		DebugPrintln(WiFi.localIP());
		if (SoftAPAlways) {
			StartAP = true; //adminenabled then always start AP
			WiFi.mode(WIFI_AP_STA);
		}
	}
	else
	{
		StartAP = true;  //start AP if cannot connect
		WiFi.mode(WIFI_AP);  //access point only....if no client connect
		DebugPrintln("Can not connect to WiFi station. Go into AP mode.");
	}

	if (StartAP)  //have option to start with AP on always?
	{

		//WiFi.mode(WIFI_AP);   

		WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
		WiFi.softAP(hostname.c_str());

		if (cDNSdisable == false) {   //if captive dns allowed
			dnsServer.setTTL(300);
			dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
			dnsServer.start(53, "*", apIP);   //start dns catchall on port 53
			dsnServerActive = true;
		}

		DebugPrint("Soft AP started IP address: ");
		DebugPrintln(WiFi.softAPIP());
		// start DNS server for a specific domain name
		//dnsServer.start(DNS_PORT, "www.setup.com", apIP);
		//dnsServer.start(DNS_PORT, "*", apIP);
		//DebugPrintln("start AP");
	} else 
		if (MDNSdisable == false) {
			MDNS.begin(DeviceName.c_str());  //multicast webname when not in SoftAP mode
			DebugPrintln("Starting mDSN " + DeviceName + ".local");
			MDNS.addService("http", "tcp", 80);
		}

//list directory
	server.on("/favicon.ico", []() { server.send(200, "text/html", "");   });
	server.on("/browse", HandleFileBrowser); 	
	server.on("/upload", HTTP_POST, []() { server.send(200, "text/plain", ""); }, handleFileUpload);
	server.on("/flashupdate", HTTP_POST, []() { server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK Restarting....wait");ESP.restart(); }, handleESPUpdate);
	server.on("/info.html", sendNetworkStatus);
	server.on("/", handleRoot);
	server.on("/jsonsave", handleJsonSave);
	server.on("/jsonload", handleJsonLoad);
	server.on("/formatspiff", formatspiffs);
	server.on("/serverlog", []() { if (isAdmin()) server.send(200, "text/html", MyWebServerLog);});
	server.on("/generate_204", handleRoot);  //use indexhtml or use embedded wifi setup...);
	server.on("/restartesp", restartESP);

	server.onNotFound([]() {
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", " FileNotFound " + server.arg(0));
	});

	CheckNewSystem();  //see if init files exist....

	server.begin();
	OTAisflashing = false;
	isDownloading = false;
	WiFi.setAutoReconnect(true);
	DebugPrintln("HTTP server started");	
	if (useNTP) {
		UDPNTPClient.begin(2390);  // Port for NTP receive
		setSyncProvider(getNtpTime);
		setSyncInterval(UpdateNTPEvery*60);  
	}


	ServerLog("SERVER STARTED");
	DebugPrintln(CurTimeString());
}



void MyWebServerClass::handle()
{
	server.handleClient();
	if (dsnServerActive)  dnsServer.processNextRequest();  //captive dns	
			
	if (millis() - lastTimeCheck >= 5000) {    //called around every second	
		//DebugPrintln(CurTimeString());
		lastTimeCheck = millis();
		//heap test
		//DebugPrintln("Free heap: " + String(ESP.getFreeHeap()));
	}  //every second timer....
		
	
}


void ICACHE_FLASH_ATTR MyWebServerClass::ServerLog(String logmsg)
{
	MyWebServerLog += "*"+String(month())+String(day())+CurTimeString()+"*" + logmsg + "<br>";
	if (MyWebServerLog.length() > 1024) { MyWebServerLog.remove(0, 256); }
}


bool MyWebServerClass::isAuthorized() {
	return isAdmin();
}


bool MyWebServerClass::WiFiLoadconfig()
{


	String values = "";
	dhcp = true;  //defaults;
	ssid = "empty";
	useNTP = false;


	File f = SPIFFS.open("/wifiset.json", "r");
	if (!f) {
		DebugPrintln("wifi config not set/found");
		return false;
	}
	else {  //file exists;
		values = f.readStringUntil('\n');  //read json         
		f.close();

		DynamicJsonBuffer jsonBuffer;


		JsonObject& root = jsonBuffer.parseObject(values);  //parse weburl
		if (!root.success())
		{
			DebugPrintln("parseObject() loadwifi failed");
			return false;
		}
		if (root["ssid"].asString() != "") { //verify good json info                                                
			ssid = root["ssid"].asString();
			password = root["password"].asString();
			if (String(root["dhcp"].asString()).toInt() == 1) dhcp = true; else dhcp = false;
			IP[0] == String(root["ip_0"].asString()).toInt(); IP[1] == String(root["ip_1"].asString()).toInt(); IP[2] == String(root["ip_2"].asString()).toInt(); IP[3] == String(root["ip_3"].asString()).toInt();
			Netmask[0] == String(root["nm_0"].asString()).toInt(); Netmask[1] == String(root["nm_1"].asString()).toInt(); Netmask[2] == String(root["nm_2"].asString()).toInt(); Netmask[3] == String(root["nm_3"].asString()).toInt();
			Gateway[0] == String(root["gw_0"].asString()).toInt(); Gateway[1] == String(root["gw_1"].asString()).toInt(); Gateway[2] == String(root["gw_2"].asString()).toInt(); Gateway[3] == String(root["gw_3"].asString()).toInt();
			if (String(root["grabntp"].asString()).toInt() == 1) useNTP = true; else useNTP = false;

			ntpServerName = root["ntpserver"].asString();

			UpdateNTPEvery = String(root["update"].asString()).toInt();
			timezone = String(root["tz"].asString()).toInt();
			DeviceName = root["devicename"].asString();
			ConfigPassword = root["AccessPass"].asString();	
			if (String(root["mDNSoff"].asString()) == "true") MDNSdisable = true; else  MDNSdisable = false;

			if (String(root["CDNSoff"].asString()) == "true") cDNSdisable = true; else  cDNSdisable = false;
			if (String(root["SoftAP"].asString()) == "true") SoftAPAlways = true; else  SoftAPAlways = false;


			if (String(root["grabntp"].asString()).toInt() == 1) useNTP = true; else useNTP = false;
			if (String(root["dst"].asString()).toInt() == 1) daylight = true; else daylight = false;					

			DebugPrintln("all good");
			return true;
		}
	} //file exists;      
}


