

#include <OneWire.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <aREST.h>

#include <SPI.h>
#include <Adafruit_MAX31855.h>

int thermoCLK = 13;
int thermoCS = 14;
int thermoDO = 12;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c,5, 4);

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "TP-LINK_A005D1";
const char* password = "?????????????";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
double ovenTemp  = 0;
int ovenRelay = 0;

// Declare functions to be exposed to the API
int ledControl(String command);

#define OvenRelayPin  15  // this is the digital pin controlling the oven relay
#define buttonPin 16


void setup()
{

 // pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
 // pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

 // pinMode(thermoCLK, INPUT);
 // pinMode(thermoCS, INPUT);
 // pinMode(thermoDO, INPUT);
  
  //thermocouple.begin(thermoCLK, thermoCS, thermoDO);
  // wait for MAX chip to stabilize
  delay(500);

 
 pinMode(buttonPin, INPUT_PULLUP);
 pinMode(OvenRelayPin, OUTPUT);

   // Initialising the UI will init the display too.
 display.init();

 display.flipScreenVertically();
 display.setFont(ArialMT_Plain_10);

   // Start up the library
 Serial.begin(115200);

 rest.variable("ovenTemp", &ovenTemp);
 rest.variable("ovenRelay", &ovenRelay);

 rest.set_id("1");
 rest.set_name("roaster");

 // Connect to WiFi
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");

 // Start the server
 server.begin();
 Serial.println("Server started");

 // Print the IP address
 String ipaddress = WiFi.localIP().toString();
 Serial.println(ipaddress);

 //Serial.println(WiFi.localIP());

 display.setTextAlignment(TEXT_ALIGN_LEFT);
 display.setFont(ArialMT_Plain_10);
 display.drawString(0, 0, "IP: " + ipaddress);
 display.display();

 showstatus();

 display.setTextAlignment(TEXT_ALIGN_LEFT);
 display.drawString(0, 10, ipaddress);

display.display();

}


void showstatus()
{  

display.clear();


//Serial.print("Internal Temp = ");
ovenTemp = thermocouple.readInternal();
//ovenTemp = thermocouple.readCelsius();
ovenRelay = digitalRead(OvenRelayPin);

//String statOven = String(digitalRead(buttonPin));
//Serial.println(statOven);

String statOven = "";

display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_10);

if (ovenRelay == 0) { statOven = "Off"; }
else { statOven = "On"; }

display.drawString(0, 10,  "ovenTemp: " +  String(ovenTemp) + " C");
display.drawString(0, 50, "ovenRelay: " + statOven );
display.display(); 
}


void loop()
{

//digitalWrite(OvenRelayPin, digitalRead(buttonPin));

int btnval = digitalRead(buttonPin);
if (btnval == LOW) {

  digitalWrite(OvenRelayPin, HIGH);
  } 
  else   {
  digitalWrite(OvenRelayPin, LOW);
  }

showstatus();
delay(5000);

// Handle REST calls
 WiFiClient client = server.available();
 if (!client) {
   return;
 }
 while(!client.available()){
   delay(1);
 }
 rest.handle(client);
}
