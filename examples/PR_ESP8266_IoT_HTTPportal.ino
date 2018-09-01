#include    "PR_ESP8266_WiFiConnectionManager.hpp"

#include    "PR_ESP8266_IoT_HTTPportal.hpp"

PR_IoT_HTTPportalClass	httpPortal;

void    setup() {
    Serial.begin(115200);
    Serial.flush();
    Serial.println("start");

    //WIFIconnectionManager.setSTAcredentials("Guest---", "123456789");
    WIFIconnectionManager.setSTAcredentials("AirS", "Ya6lok0va$");
	//WIFIconnectionManager.setSTAcredentials("TP-LINK_B1DF34", "EDB1DF34");
    WiFi.mode(WIFI_STA);
    Serial.println("SET STA");
	
	httpPortal.start();
	
}

void    loop() {
	
    WIFIconnectionManager.loop();
    
	httpPortal.loop();

  }
	
	
	
