#include <Adafruit_NeoPixel.h>

#define PIN D3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, PIN, NEO_GRB + NEO_KHZ800);

typedef enum 
{
  eNone=0,
  eRainbow=1,
  eRainbowCycle=2,
  eColorWipe=3,
  eTheaterChase=4,
  ePIR=5
} LedFuncs;

int LedFunc = eNone;
int PrevLedFunc = eNone;
int i,j;
uint16_t red, green, blue, wait;
uint32_t a,b,c,d;

void rainbow();
void rainbowCycle();
void colorWipe();
void theaterChase();
void theaterChaseRainbow();
uint32_t Wheel(byte WheelPos);

void LedStripLoop()
{
  PrintTitle("#"+LedFunc,0);
  delay(1000);
  switch (LedFunc)
  {
    case eRainbow:
      PrintTitle("Rainbow",0);
      rainbow();
      break;
    case eRainbowCycle:
      rainbowCycle();
      break;
    case eColorWipe:
      colorWipe();
      break;
    case ePIR:
      if (digitalRead(PIR)==HIGH) {
        red=0;green=255;blue=0;
      }else{
        red=0;green=0;blue=255;
      }
      setColor();
      break;
    default:
      break;
  }
  if(i>=strip.numPixels()){
    i=0;
    a = c;
    c=b;
    b=a;
  }else{
    i++;
  }
  if(j>=256){
    j=0;
  }else{
    j++;
  }
}

void LedStripUpdateArgs(){
  if (server.arg("red")=="") red = 255;
  else red  = server.arg("red").toInt();
  if (server.arg("green")=="") green = 0;
  else green = server.arg("green").toInt();
  if (server.arg("blue")=="") blue = 0;
  else blue = server.arg("blue").toInt();
  if (server.arg("wait")=="") wait = 10;
  else wait = server.arg("wait").toInt();
}

void setColor(){
  for( j=0; j<strip.numPixels(); j++)
  {
    strip.setPixelColor(j,strip.Color(red,green,blue));
  }
  strip.show();
}

void colorWipe() {
    strip.setPixelColor(i, c);
    strip.show();
}

void rainbow() {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    strip.show();
  }
}

//Theatre-style crawling lights.
void theaterChase() {
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
void theaterChaseRainbow() {
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
