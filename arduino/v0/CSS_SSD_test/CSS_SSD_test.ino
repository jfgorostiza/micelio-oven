/*********************************************************************
Mezcla de los dos test o ejemplos de manejo de la pantalla y del sensor CCS

*********************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include "Adafruit_CCS811.h"

// Used for I2C or SPI
#define OLED_RESET -1

Adafruit_SSD1305 display(128, 64, &Wire, OLED_RESET);
Adafruit_CCS811 ccs;

void setup()   {                
  Serial.begin(9600);
  while (! Serial) delay(100);
  Serial.println("SSD1305 OLED and CCS sensor test");

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
  ccs.setDriveMode( CCS811_DRIVE_MODE_250MS ); // 0x04
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
  Serial.println(temp);

  
  if ( ! display.begin(0x3C) ) {
     Serial.println("Unable to initialize OLED");
//     while (1) yield();
  }
  display.display(); // show splashscreen
  delay(1000);
  display.clearDisplay();   // clears the screen and buffer

  display.setRotation(0);
  display.setTextSize(1);
//  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello World");
  display.display(); // show splashscreen
  delay(1000);

}

void loop() {
  if(ccs.available()) {
    if(!ccs.readData()) {
      float temp = ccs.calculateTemperature();
      float eCO2 = ccs.geteCO2();
      float TVOC = ccs.getTVOC();
      Serial.print("CO2: ");
      Serial.println(eCO2);
      Serial.print("ppm, TVOC: ");
      Serial.println(TVOC);
      Serial.print("ppb, Temp:");
      Serial.println(temp);

      display.setCursor(0,0);
      display.print("CO2: ");
      display.println(eCO2);
      display.setCursor(0,10);
      display.print("ppm, TVOC: ");
      display.println(TVOC);
      display.display(); // show splashscreen
    } else {
//      Serial.println("ERROR!");
      delay(60);
//      while(1);
    }
  }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (uint8_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (uint16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) {
  for (uint8_t i=0; i<display.height()/3-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (uint8_t i=0; i<display.height()/3-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
   
void testdrawrect(void) {
  for (uint8_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}

void testdrawline() {  
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (uint8_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int8_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int8_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int8_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (uint8_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (uint8_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(250);
}