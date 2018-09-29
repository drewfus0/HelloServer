#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>   // Include the SPIFFS library
//LED STRIP STUFF
#include <Adafruit_NeoPixel.h>
#define PIN D3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, PIN, NEO_GRB + NEO_KHZ800);
#include "LedStrip.h"
#define LED D4

//const char* ssid = "FTelstra9EB5CF";
//const char* password = "8D22AA74CF";
const char* ssid = "OPTUS_2776DC";
const char* password = "kythewhiff73073";

ESP8266WebServer server(80);

const int led = LED;
uint16_t red, green, blue, wait;
uint32_t c,b;
uint16_t j=0, i=0;
bool isRainbow=false, isWipe=false;

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

void handleRoot() {
  digitalWrite(led, 0);
  if(!handleFileRead("/index.html")){
    server.send(200, "text/plain", "hello from esp8266!");
  }
  digitalWrite(led, 1);
}

void handleNotFound() {
  digitalWrite(led, 0);
  if (!handleFileRead(server.uri())) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  }
  digitalWrite(led, 1);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/rainbow", []() {
    handleRoot();
    UpdateArgs();
    rainbowCycle(wait);
    isRainbow=true;
    isWipe=false;
  });

  server.on("/wipe", []() {
    handleRoot();
    UpdateArgs();
    i=0;
    c=strip.Color(255,0,0);
    b=strip.Color(0,0,0);
    isRainbow=false;
    isWipe=true;
    });
  
  server.on("/off", [](){
    handleRoot();
    UpdateArgs();
    isRainbow=false;
    isWipe=false;
    red=0;green=0;blue=0;
    setColor(wait);
  });
  // Start the SPI Flash Files System 
  SPIFFS.begin();
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void) {
  server.handleClient();
  if (isRainbow) rainbowCycle(wait);
  if (isWipe) colorWipe(wait);
}

//########################################################################
//  SPIFFS handle html files
//########################################################################
String getContentType(String filename){
  if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                         // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}

//########################################################################
//  LedFunctions
//########################################################################
// Fill the dots one after the other with a color

void UpdateArgs(){
      if (server.arg("red")=="") red = 255;
    else red = server.arg("red").toInt();
    if (server.arg("green")=="") green = 0;
    else green = server.arg("green").toInt();
    if (server.arg("blue")=="") blue = 0;
    else blue = server.arg("blue").toInt();
    if (server.arg("wait")=="") wait = 2;
    else wait = server.arg("wait").toInt();
}

void setColor(uint8_t wait){
  for( j=0; j<strip.numPixels(); j++)
  {
    strip.setPixelColor(j,strip.Color(red,green,blue));
  }
  strip.show();
  delay(wait);
}

void colorWipe(uint8_t wait) {
  //for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    if(i>=strip.numPixels()){
      i=0;
      uint32_t a = c;
      c=b;
      b=a;
    }else{
      i++;
    }
    delay(wait);
  //}
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



// Slightly different, this makes the rainbow equally distributed throughout

void rainbowCycle(uint8_t wait) {
  //uint16_t i, j;
  
  if (i>=strip.numPixels()) {
    i=0;
    j++;
  }
  if (j>=256*5){
    j=0;
  }
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
  }
  strip.show();
  delay(wait);
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}