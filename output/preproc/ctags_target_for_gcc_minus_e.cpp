# 1 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino"
# 1 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino"

# 3 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2
# 4 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2
# 5 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2
# 6 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2
# 7 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2
//LED STRIP STUFF
# 9 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2

Adafruit_NeoPixel strip = Adafruit_NeoPixel(295, D3, ((3 << 6) | (0 << 4) | (1 << 2) | (2)) + 0x0000 /* 800 KHz datastream*/);
# 12 "c:\\Users\\drewf\\Documents\\Arduino\\HelloServer\\HelloServer.ino" 2


const char* ssid = "FTelstra9EB5CF";
const char* password = "8D22AA74CF";

ESP8266WebServer server(80);

const int led = 13;

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path); // send the right file to the client (if it exists)

void handleRoot() {
  digitalWrite(led, 1);
  if(!handleFileRead("index.html")){
    server.send(200, "text/plain", "hello from esp8266!");
  }
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, 0x01);
  digitalWrite(led, 0);
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

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
    rainbow(20);
  });

  server.on("/", []() {
    server.send(200, "text/plain", "this Also works!");
    Wheel(57);
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

bool handleFileRead(String path){ // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html"; // If a folder is requested, send the index file
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){ // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz)) // If there's a compressed version available
      path += ".gz"; // Use the compressed version
    File file = SPIFFS.open(path, "r"); // Open the file
    size_t sent = server.streamFile(file, contentType); // Send it to the client
    file.close(); // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false; // If the file doesn't exist, return false
}

//########################################################################
//  LedFunctions
//########################################################################
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
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
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) { //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0); //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) { // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0); //turn every third pixel off
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
