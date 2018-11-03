/**
* esp8266 doorbell code
* Niall McAndrew 27May2017, 3Nov2018
* 
* NodeMCU 1.0 (ESP-12E Module), 80 MHz, Flash, 4M (no SPIFFS), 
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

int dbvolts = 0;
int dbpressed = 0;
int dbalert = 0;

void setup() {

    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("TP-LINK_A005D1", "??????????????/");
}

void loop() {
    // wait for WiFi connection
    
      dbvolts = analogRead(A0);
      delay(1000);
      Serial.println(dbvolts);
      
      if((WiFiMulti.run() == WL_CONNECTED)) 
      {     
        if  (dbvolts > 700)
              {
                if (dbalert !=1)
                  {
                  // button is pressed, and alert hasn't been successfully sent yet
                  USE_SERIAL.print("DOORBELL PRESSED !!!");
      
                  USE_SERIAL.print("[HTTP] begin...\n");
                  http.begin("http://192.168.1.113/html/snap.php"); 
          
                  USE_SERIAL.print("[HTTP] GET...\n");
                  // start connection and send HTTP header
                  int httpCode = http.GET();
          
                  // httpCode will be negative on error
                  if(httpCode > 0) 
                  {
                  // HTTP header has been send and Server response header has been handled
                  USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
      
                      // file found at server
                      if(httpCode == HTTP_CODE_OK) 
                        {
                        dbalert = 1;  // doorbell was pressed, and alert successfully sent
                        String payload = http.getString();
                        USE_SERIAL.println(payload);
                        
                        }
                   } 
                   else  
                      {
                      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                      dbalert = 0;
                      delay (2000);   // wait 2 seconds before trying again
                      }
                  http.end();
                  //delay(5000);
                  }
                            
               }
               else 
               {
                dbalert = 0;
                }
         }
}
