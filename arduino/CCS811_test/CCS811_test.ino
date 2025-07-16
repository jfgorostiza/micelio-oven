/***************************************************************************
  This is a library for the CCS811 air

  This sketch reads the sensor

  Designed specifically to work with the Adafruit CCS811 breakout
  ----> http://www.adafruit.com/products/3566

  These sensors use I2C to communicate. The device's I2C address is 0x5A

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;

void setup() {
  Serial.begin(9600);

  Serial.println("CCS811 test");

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
//  ccs.SWReset();
  ccs.setDriveMode( CCS811_DRIVE_MODE_250MS ); // 0x04
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
  Serial.println(temp);
}

void loop() {
  if(ccs.available()){
    int r = ccs.readData();
    if(!r){
      float temp = ccs.calculateTemperature();
      float eCO2 = ccs.geteCO2();
      float TVOC = ccs.getTVOC();
      Serial.print("CO2: ");
      Serial.println(eCO2);

      Serial.print("ppm, TVOC: ");
      Serial.println(TVOC);

      Serial.print("ppb, Temp:");
      Serial.println(temp);
    }
    else{
      Serial.println("ERROR!");
//      Serial.println(r);
//      Serial.println(ccs.checkError());
  //    while(1);
    }
  } else {
//    Serial.println("CSS not available");
    delay(250);
  }
//  delay(500);
}
