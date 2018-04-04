
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <ESP8266WiFi.h>
//#include <WiFi.h>

#include "SSD1306.h"

// curl 'http://192.168.1.162/mode/12/o'
// curl 'http://192.168.1.162/digital/12/0'


// Initialize the OLED display using Wire library
SSD1306  display(0x3c,5, 4);

#include <WiFi.h>
#include <aREST.h>


// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "TP-LINK_A005D1";
const char* password = "????????";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
String temperature;
int BathroomTemp = 17;
int BathroomHumid = 67;
int immersionTemp = 25;
int immersionStatus = 0;
int BathroomFan = 0;


// Declare functions to be exposed to the API
int ledControl(String command);

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 15   // was 3
#define BoilerRelayPin 12  // was 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#include <Wire.h>

void setup()
{
    // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
    // Start up the library
  Serial.begin(115200);

  pinMode(BoilerRelayPin, INPUT);  
  rest.variable("temperature",&temperature);
  
  rest.variable("BathroomTemp", &BathroomTemp);
  rest.variable("BathroomHumid", &BathroomHumid);
  rest.variable("immersionTemp", &immersionTemp);
  rest.variable("immersionStatus", &immersionStatus);
  rest.variable("BathroomFan", &BathroomFan);
    
  rest.set_id("1");
  rest.set_name("htpres");

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

  
  Serial.println("Immersion");
  sensors.begin();
  Wire.begin();  

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, ipaddress);

 display.display();
 
 // oled.init();                      // Initialze SSD1306 OLED display
 // oled.clearDisplay();              // Clear screen
 // oled.setTextXY(0,0);              // Set cursor position, start of line 0
 // oled.putString("Immersion");
 // oled.setTextXY(1,0);              // Set cursor position, start of line 1
  //oled.putString(ipaddress);
//  oled.setTextXY(2,0);              // Set cursor position, start of line 2
//  oled.putString("Pasadena,");
//  oled.setTextXY(2,10);             // Set cursor position, line 2 10th character
//  oled.putString("CA");
}



void showstatus()
{  

 String statFan = "";
 String statimmersion = "";
 display.setTextAlignment(TEXT_ALIGN_LEFT);
 display.setFont(ArialMT_Plain_10);

 if (BathroomFan == 0) { statFan = "Off"; }
 else { statFan = "On"; }

 if (immersionStatus == 0) { statimmersion = "Off"; }
 else { statimmersion = "On"; }
 
 display.drawString(0, 10,  "Bathroom Temp: " +  String(BathroomTemp) + " C");
 display.drawString(0, 20, "Bathroom Humid: " + String(BathroomHumid) + "%" );
 display.drawString(0, 30, "Bathroom Fan: " + statFan);
 display.drawString(0, 40, "Immersion Temp: " +  String(immersionTemp) + " C");
 display.drawString(0, 50, "Immersion Status: " + statimmersion );
 display.display(); 
}


void loop()
{

//  display.clear();
  // draw the current demo method
//  demos[demoMode]();

//  display.setTextAlignment(TEXT_ALIGN_RIGHT);
//  display.drawString(10, 128, "Hello");
  // write the buffer to the display
  
  
  Serial.print("Requesting temperatures...");
  delay(2000);
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  //Serial.println(sensors.getTempCByIndex(0));  
  String temp = String(sensors.getTempCByIndex(0)) + " deg. C";
  Serial.println(temp);
  temperature = temp;
  //oled.clearDisplay();              // Clear screen
  //oled.setTextXY(0,0);              // Set cursor position, start of line 0
  //oled.putString("Immersion");
  //oled.setTextXY(1,0); 
  //oled.putString(temp);
  //oled.setTextXY(2,0); 
  //oled.putString("Boiler : " );
  Serial.println("BoilerRelayPin: " + (String(digitalRead(BoilerRelayPin))));
  int val = digitalRead(BoilerRelayPin);
  //oled.putString(String(val));
  
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
