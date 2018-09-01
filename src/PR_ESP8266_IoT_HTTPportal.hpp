#pragma once

#include <arduino.h>
//#include <string.h>
#include <Esp.h>                    // core

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

#include <PR_GetTau.hpp>

#include <PR_HTMLpageConstructor.hpp>

class PR_IoT_HTTPportalClass {
	public:
		
		bool	start();
		bool	stop();
			
		void	loop();
		
		void	sendHtml(String& page); 
		
		
		
		void	handleNotFound();
		void	handleRoot();
		void	handleReset();
		void	handleInfo();
		void	handleExit();
		
				
	protected:
		
		void	_setupHandles();
		
		std::unique_ptr<DNSServer>        dnsServer;
		std::unique_ptr<ESP8266WebServer> httpServer;
	
		bool	_isStarted = false;	

		const byte    DNS_PORT = 53;
		
		//helpers
			
		template <typename Generic>
		void	DEBUG_WM(Generic text);
	
};

bool	PR_IoT_HTTPportalClass::start() {
	
	if ( _isStarted ) return true;
	
	dnsServer.reset(new DNSServer());
	httpServer.reset(new ESP8266WebServer(80));
	
	/* Setup the DNS httpServer redirecting all the domains to the apIP */
	dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());
	
	// Setup web pages: root, wifi config pages, SO captive portal detectors and not found. 
	_setupHandles();
	
	httpServer->begin(); // Web httpServer start
	DEBUG_WM(F("HTTP httpServer started"));	
	
	_isStarted = true;
	
	return true;
}

bool	PR_IoT_HTTPportalClass::stop() {
	
	httpServer->close();
	//httpServer.reset();//?
	DEBUG_WM(F("HTTP httpServer stoped"));	
	
	_isStarted = false;
	
	return true;
}

void	PR_IoT_HTTPportalClass::loop() {
	
	if ( _isStarted ) {
		dnsServer->processNextRequest();
		httpServer->handleClient();	
	}
}

void PR_IoT_HTTPportalClass::sendHtml(String& page) {
	
	if ( _isStarted ) {
		httpServer->send(200, "text/html", page);  
	}	

}

////////////////////////////////////////////////////////////////////////////

void	PR_IoT_HTTPportalClass::_setupHandles() {

	// Setup common portal pages:
	httpServer->onNotFound (	std::bind(&PR_IoT_HTTPportalClass::handleNotFound, this) );
	httpServer->on("/",			std::bind(&PR_IoT_HTTPportalClass::handleRoot, this) );
	httpServer->on("/reset",	std::bind(&PR_IoT_HTTPportalClass::handleReset, this) );
	httpServer->on("/info",		std::bind(&PR_IoT_HTTPportalClass::handleInfo, this) );
	httpServer->on("/exit",		std::bind(&PR_IoT_HTTPportalClass::handleExit, this) );
	
	
	// add user pages here
	
	//from ESP8266_WiFiConnectionManager
	//httpServer->on("/wifiscan", std::bind(&::handleWifiScan, this));
	//httpServer->on("/wifisave", std::bind(&::handleWifiSave, this));
	
	//from ESP8266_mqttConnectionManager
	//httpServer->on("/mqtt", 	std::bind(&PR_IoTConnectionManager::handleMqtt, this));
	//httpServer->on("/mqttsave", std::bind(&::handleMqttSave, this));
	//httpServer->on("/r", 		std::bind(&::handleReset, this));

}


////////////////////////////////////////////////////////////////////////////////////////////////
////	Portal common PAGES


void PR_IoT_HTTPportalClass::handleRoot() {

	static const char  F_ROOT[] PROGMEM	= \
"<p>Portal standart pages:</p> \
<p><a href=\"/info\">  info</a> </p> \
<p><a href=\"/reset\"> reset</a> </p> \
<p><a href=\"/exit\">  exit</a> </p> \
<p><a href=\"/index\"> index</a> </p>";  
  
	DEBUG_WM(F("Handle root"));
	
	HTMLpageConstructorClass	page("root");
	page.addBodyText( FPSTR(F_ROOT) );
	
	sendHtml(page.html); 
}


void PR_IoT_HTTPportalClass::handleNotFound() {

	String message = "File Not Found\n\n";
	message += "URI: ";
	message += httpServer->uri();
	message += "\nMethod: ";
	message += (httpServer->method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += httpServer->args();
	message += "\n";
	for (uint8_t i=0; i<httpServer->args(); i++) {
		message += " " + httpServer->argName(i) + ": " + httpServer->arg(i) + "\n";
	}
	
	httpServer->send(404, "text/plain", message);
}

void PR_IoT_HTTPportalClass::handleReset() {
	DEBUG_WM(F("Reset"));

	httpServer->send(200, "text/plain", F("Module will reset in a few seconds.") );

	DEBUG_WM(F("Sent reset page"));

	delay(5000);
	ESP.reset();
	delay(2000);
}

void PR_IoT_HTTPportalClass::handleExit() {
	
	DEBUG_WM(F("Exit"));
 
	httpServer->send(200, "text/plain", F("Portal stopped") );

	stop();
}

void PR_IoT_HTTPportalClass::handleInfo() {
	DEBUG_WM(F("info page"));

	HTMLpageConstructorClass page("ESP8266 info");
	
	page.addBodyText( F("vdhgsdfhdfjhgd") );
 	
	
	
	sendHtml(page.html);

}























///////////////////////////////////////////////////////////////////////

template <typename Generic>
void PR_IoT_HTTPportalClass::DEBUG_WM(Generic text) {
    Serial.print("*WM: ");
    Serial.println(text);
}


