#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>   // Include the SPIFFS library

#include "OLed.h"
#include "LedStrip.h"
#define LED D4
#define PIR D5

//const char* ssid = "FTelstra9EB5CF";
//const char* password = "8D22AA74CF";
const char* ssid = "OPTUS_2776DC";
const char* password = "kythewhiff73073";

ESP8266WebServer server(80);

const int led = LED;


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
  pinMode(PIR, INPUT);
  dispSetup();
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

  if (MDNS.begin("esp8266.local")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.on("/rainbow", []() {
    handleRoot();
    i=0;j=0;
    LedFunc = eRainbow;
    UpdateArgs();
  });
  
  server.on("/pir", []() {
    handleRoot();
    LedFunc = ePIR;
    UpdateArgs();
  });
  
  server.on("/wipe", []() {
    handleRoot();
    c=strip.Color(255,0,0);
    b=strip.Color(0,0,0);
    i=0;
    LedFunc =eColorWipe;
    UpdateArgs();
  });
  
  server.on("/off", [](){
    handleRoot();
    red=0;
    green=0;
    blue=0;
    LedFunc=eNone;
    UpdateArgs();
    setColor();
  });
  // Start the SPI Flash Files System 
  SPIFFS.begin();
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  strip.begin();
  strip.show();

  
}

void loop(void) {
  server.handleClient();
  LedStripLoop();
  delay(wait);
}

void UpdateArgs(){
  LedStripUpdateArgs();
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

