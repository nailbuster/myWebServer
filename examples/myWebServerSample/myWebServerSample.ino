
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <ArduinoJson.h> 
#include <myWebServer.h>
#include <TimeLib.h>



void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);

	MyWebServer.begin();	
}

void loop() {
  // put your main code here, to run repeatedly:
	MyWebServer.handle();

}
