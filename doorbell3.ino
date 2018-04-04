/**
* esp8266 doorbell code
* Niall McAndrew 27May2017
* 
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

int dbvolts = 0;
int dbpressed = 0;

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
    WiFiMulti.addAP("TP-LINK_A005D1", "???????????");
}

void loop() {
    // wait for WiFi connection
    
      dbvolts = analogRead(A0);
      delay(1000);
      Serial.println(dbvolts);
      
      if((WiFiMulti.run() == WL_CONNECTED)) 
      {     
         if ((dbvolts > 400) && (dbpressed != 1))
              {
              dbpressed = 1;

              HTTPClient http;
      
              USE_SERIAL.print("[HTTP] begin...\n");
              //http.begin("http://192.168.1.113:2468/doorbell/1/on"); //HTTP
              http.begin("http://192.168.1.113/html/snap.php"); //HTTP
      
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
                        String payload = http.getString();
                        USE_SERIAL.println(payload);
                        }
                   } 
                   else  
                      {
                      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                      }
              http.end();
              delay(5000);
              }
          if ((dbvolts < 400) && (dbpressed !=0))
            {
            dbpressed = 0;
            // publish dbpressed
            Serial.print(F("\nSending dbpressed val "));
            Serial.print(dbpressed);
            Serial.print("...");
            }    
      }
}

