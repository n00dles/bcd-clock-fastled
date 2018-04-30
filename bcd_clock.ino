#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

#include "FastLED.h"

#include <WebSockets.h>           //https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>

// How many leds in your strip?
#define NUM_LEDS 20

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 5
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

// color for LEDS 
uint8_t cR = 254; 
uint8_t cG = 0; 
uint8_t cB = 0; 

uint8_t brightness = 100; 

// reverse binary number for numbers 0-9
uint8_t  bits[10][5] = {
 {0,0,0,0,0},
 {1,0,0,0,0},
 {0,1,0,0,0},
 {1,1,0,0,0},
 {0,0,1,0,0},
 {1,0,1,0,0},
 {0,1,1,0,0},
 {1,1,1,0,0},
 {0,0,0,1,0},
 {1,0,0,1,0}
};

// neopixel led number for start of each column and the length in pixels
uint8_t digits[6][2]={
  {0,4},
  {4,3},
  {7,4},
  {11,3},
  {14,4},
  {18,2}
  };

// display time in 12/24 hour format
bool twenty4hour = false;

// second timer
unsigned long oneSecondLoopDue = 0;

int timeHour;
int timeMinutes;
int timeSeconds;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#include <Timezone.h>    // https://github.com/JChristensen/Timezone Modified!

// For Webserver
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// For WifiManager
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


#include <WiFiClientSecure.h>


// From World clock example in timezone library
// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
Timezone UK(BST, GMT);

TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    // UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    // UTC + 10 hours
Timezone ausET(aEDT, aEST);

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);


void drawDigit(uint8_t digit, uint8_t val){
  // get the column position and number of pixels
  uint8_t dpos = digits[digit][0];
  uint8_t lnt = digits[digit][1];
  for (uint8_t i=0;i<lnt;i++){
    if (bits[val][i] == 1){
      leds[dpos+i].setRGB(cR, cG, cB);
    } else {
      leds[dpos+i].setRGB(0, 0, 0);
    }
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
}


void setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(brightness);
  Serial.begin(115200);
  Serial.println("Starting....");

  clearLeds(); 
  
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("BCDClock", "password");

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");

  IPAddress ipAddress = WiFi.localIP();
  Serial.println(ipAddress);

  timeClient.begin();

}

void clearLeds(){
  for (uint8_t i=0;i<=20;i++){
    leds[i].setRGB(0,0,0);
  }
  FastLED.show();
}

void loop() { 
  uint8_t tim = 0; 
  unsigned long now = millis();
  if (now > oneSecondLoopDue) {
    timeClient.update();
    displayTime();
    oneSecondLoopDue = now + 1000;
  }
}


// gets the current time and displays it in each column. 
void displayTime() {

  unsigned long epoch = UK.toLocal(timeClient.getEpochTime());

  timeHour = (epoch  % 86400L) / 3600;
  timeMinutes = (epoch % 3600) / 60;
  timeSeconds = (epoch % 60);

  Serial.print(timeHour);
  Serial.print(timeMinutes);
  Serial.print(timeSeconds);
  
  if (twenty4hour == true) {
    if (timeHour >=13 && timeHour<=24){
      timeHour = timeHour - 12;
    }
  }

  if (timeHour < 10) {
    drawDigit(5,0);
    drawDigit(4,timeHour);
  } else {
    drawDigit(5,timeHour / 10);
    drawDigit(4,timeHour % 10);
  }

  if (timeMinutes < 10) {
    drawDigit(3,0);
    drawDigit(2,timeMinutes);
  } else {
    drawDigit(3,timeMinutes / 10);
    drawDigit(2,timeMinutes % 10);
  }

  if (timeSeconds < 10) {
    drawDigit(1,0);
    drawDigit(0,timeSeconds);
  } else {
    drawDigit(1,timeSeconds / 10);
    drawDigit(0,timeSeconds % 10);
  }
  
  FastLED.show(); 
}
