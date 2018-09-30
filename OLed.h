  #include <Wire.h>  // Include Wire if you're using I2C
  #include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
  #define PIN_RESET 255  //
  #define DC_JUMPER 0 // I2C Addres: 0 - 0x3C, 1 - 0x3D
  MicroOLED disp(PIN_RESET,DC_JUMPER);
  
void dispSetup()
{
  disp.begin();
  disp.clear(ALL);  
  disp.display();
}

void PrintTitle(String title, int font)
{
  int middleX = disp.getLCDWidth() / 2;
  int middleY = disp.getLCDHeight() / 2;
  
  disp.clear(PAGE);
  disp.setFontType(font);
  // Try to set the cursor in the middle of the screen
  disp.setCursor(0,0);
  // Print the title:
  disp.println(title);
  disp.println(WiFi.localIP().toString());
  disp.display();
  disp.clear(PAGE);
}