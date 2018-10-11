#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    wifiManager.resetSettings();
    
    //set custom ip for portal
    wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("Smoke");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    if(WiFi.status() == WL_CONNECTED){
      String payload;
      do
      {
        HTTPClient http;
  
        http.begin("http://quiet-journey-37928.herokuapp.com/arduino");
        http.addHeader("Content-Type", "text/plain");
    
        int httpCode = http.POST("hola fede");
        payload = http.getString();
    
        Serial.print("http result: ");
        Serial.println(httpCode);
        Serial.print("payload: ");
        Serial.println(payload);
    
        http.end();

        delay(5000);
      }while (payload != "Llego alerta! ");
    }else{
      Serial.println("Error in WiFi connection");
    }
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

      http.begin("http://quiet-journey-37928.herokuapp.com/arduino");
  
      int httpCode = http.GET();

      if(httpCode > 0) {
        String payload = http.getString();
        Serial.print("payload: ");
        Serial.println(payload);
      }
  
      http.end();
      
  }else{
    Serial.println("Error in WiFi connection");
  }
      
    delay(5000);
    
}
