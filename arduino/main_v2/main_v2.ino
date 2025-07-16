/**
v2

- unión con la Raspi para el envío de datos (18 int)
dht1_T, dht2_T, dht3_T, dht1_H, dht2_H, dht3_H, hm, tm, CO2, eCO2, TVOC, t_min, t_max, h_min, h_max, heat_rele, hum_rele, fan_rele
 
- control de actuadores HEAT y HUM: PID.
    Intensidad del actuador: Hacer una PWM de baja frecuencia (T = 4secs) para los relés. 
 - lectura sensor MZH (CO2_PIN) a través de un puerto serie secundario
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include "Adafruit_CCS811.h"
#include "DHT.h"

// Sensors
#define HUM_PIN 2
#define DHT11_1 3
#define DHT11_2 4
#define DHT22_PIN 5
#define CO2_PIN 6

// Used for I2C or SPI
#define OLED_RESET 9  //13
#define OLED_DC 10    //8
#define OLED_MOSI 11  //11
#define OLED_CS 12    //10
#define OLED_CLK 13   //13

// Actuators
#define HUM_RELE_PIN 49
#define FAN_LED_PIN 52
#define FAN_PIN 53
#define HEAT_LED_PIN  50
#define HEAT_PIN  51

Adafruit_SSD1305 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

Adafruit_CCS811 ccs;
DHT dht1(DHT11_1, DHT11); // DHT22
DHT dht2(DHT11_2, DHT11); // DHT22
DHT dht3(DHT22_PIN, DHT22); // DHT22

float h[3], t[3], hic[3];
float hm, tm, hicm;   // valor medio utilizado para el control

float t_min = 24.0; // umbrales de control
float t_max = 25.0;
float h_min = 40; // umbrales de control
float h_max = 100;

bool heatState = false; // estado de los calentadores
bool humState = false;  // estado del humidificador
bool fanState = false;  // estado del humidificador

float eCO2; // sensor CCS-811
float TVOC;
long CO2; // sensor MH-Z14

void setup() {
  Serial.begin( 115200 );
  while( !Serial );
  
  /* Heater */
  pinMode( HEAT_PIN, OUTPUT );
  digitalWrite( HEAT_PIN, HIGH );
  pinMode( HEAT_LED_PIN, OUTPUT );
  digitalWrite( HEAT_LED_PIN, LOW );
  
  /* Fan */
  pinMode( FAN_PIN, OUTPUT );
  digitalWrite( FAN_PIN, HIGH );
  pinMode( FAN_LED_PIN, OUTPUT );
  digitalWrite( FAN_LED_PIN, LOW );

  /* Humidifator */  
  pinMode( HUM_RELE_PIN, OUTPUT );
  pinMode( HUM_PIN, OUTPUT );

  if ( !display.begin(0x3C) ) {
     //Serial.println("Unable to initialize OLED");
     eternalError();     
  } else {//Serial.println("OK");
    display.clearDisplay();   // clears the screen and buffer
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println("Micelio-v2");
    display.display(); 
    delay(1000); 
  }
  /* sensor CO2 */
  pinMode( CO2_PIN, INPUT );

  //Serial.println("Sensor CSS...");
  if(!ccs.begin()){
    //Serial.println("Failed to start CCS811 sensor! Please check your wiring.");
    eternalError();
//  } else //Serial.println("OK");
  }
  //Serial.println( "DHT_1 - begin" );
  dht1.begin();
  //Serial.println( "DHT_2 - begin" );
  dht2.begin();
  //Serial.println( "DHT_3 - begin" );
  dht3.begin();
  readDHT();

  //Serial.println("CCS811 temp offset:");
  if( waitCCSsensor( 10000 ) == 0 ) {
    ccs.setDriveMode( CCS811_DRIVE_MODE_250MS ); // 0x04
    float temp = ccs.calculateTemperature();
    ccs.setTempOffset(temp - tm);
    //Serial.println(temp);
  } else {
    //Serial.println("CCS sensor not available");
    eternalError();
  }
  delay(1000);
}

void loop() {
  t_min = map(analogRead(A1), 0, 1023, 15, 50);
  t_max = map(analogRead(A0), 0, 1023, 15, 50);
  h_min = map(analogRead(A2), 0, 1023, 10, 90);
  h_max = map(analogRead(A3), 0, 1023, 10, 90);
  readDHT();
  readCCS();
  readCO2();
  displayValues();
  
  // Control
  if( tm < t_min ) { // Enciende el calentador
    //Serial.println("Hace FRIO");
    heatOn();
  } else {
    heatOff();
  }
  if( tm > t_max ) { // Apaga el calentador y enciende el ventilador
    //Serial.println("Hace CALOR");
    heatOff();
    fanOn();
  } else {
    fanOff();
  }

  if( hm < h_min ) { // Enciende el humidificador
    //Serial.println("Estamos SECOS");
    humOn();
  } else {
    humOff();
  }
  
  if( hm > h_max ) { // Apaga humidificador
    //Serial.println("Estamos HUMEDOS");
    humOff();
    fanOn();
  } 

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Leer el comando completo hasta un salto de línea
    if (command == "GET_DATA") {
      sendData();
    }
  }
  delay(10);
} // loop

void heatOn(){
  digitalWrite( HEAT_PIN, LOW );
  digitalWrite( HEAT_LED_PIN, HIGH );
  heatState = true;
}

void heatOff(){
  digitalWrite( HEAT_PIN, HIGH );
  digitalWrite( HEAT_LED_PIN, LOW );
  heatState = false;
}

void humOn() {
  digitalWrite( HUM_RELE_PIN, LOW ); 
  digitalWrite( FAN_LED_PIN, HIGH ); 
  humState = true;
}

void humOff(){
  digitalWrite( HUM_RELE_PIN, HIGH );
  digitalWrite( FAN_LED_PIN, LOW );
  humState = false;
}

void fanOn() {
  digitalWrite( FAN_PIN, LOW ); // enciende fan
  fanState = true;
}

void fanOff() {
  digitalWrite( FAN_PIN, HIGH ); // apaga fan
  fanState = false;  
}

void sendData() {
  int data[18] = {t[0], t[1], t[2], h[0], h[1], h[2], hm, tm, CO2, eCO2, TVOC, t_min, t_max, h_min, h_max, heatState, humState, fanState};
 for (int i = 0; i < 18; i++) {
    Serial.print(data[i]);
    if (i < 17) {
      Serial.print(",");  // Separador por coma
    }
  }
  Serial.println();  // Nueva línea al final de los datos
}

void eternalError() {
  while(1){
   digitalWrite( HEAT_LED_PIN, HIGH );
   delay(500);
   digitalWrite( HEAT_LED_PIN, LOW );
   delay(500);    
  }
}

void readCO2(){
  while(digitalRead(CO2_PIN) == LOW) {;}
  long down = millis();
  //TODO poner un timeout
  while(digitalRead(CO2_PIN) == HIGH) {;}
  long top = millis();
  long down_time = top - down;
  //TODO poner un timeout
  while(digitalRead(CO2_PIN) == LOW) {;}
  down = millis();
  long top_time = down - top;
// Validity check high+low = 1004 (small margin or error allowed -> 990)
  if(top_time + down_time > 990) {
    CO2 = top_time * 2;
  } else {
// Output in case of invalid check: value instead of 1004 ms
    CO2 = -(top_time + down_time);
  }
}

bool waitCCSsensor( long timeOut ) {
  long t0 = millis();
  while(!ccs.available()) {
    if((millis() - t0) > timeOut) return -1;
  }
  return 0;
}

void displayValues() {
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(2);
  display.setTextColor(WHITE);
 
  display.setCursor(2,0);
  display.print(round(tm));
  display.setTextSize(1);
  display.print("C");
  display.setTextSize(2);
  display.println(":");

  display.setCursor(50,0);
  display.print(round(t_min));
  display.setTextSize(1);
  display.print("C");
  display.setTextSize(2);
  display.print("|");
  display.setCursor(90,0);
  display.print(round(t_max));
  display.setTextSize(1);
  display.println("C");

  display.setCursor(2,20);
  display.setTextSize(2);
  display.print(round(hm));
  display.setTextSize(1);
  display.print( "%" );
  display.setTextSize(2);
  display.println( ":" );

  display.setCursor(50,20);
  display.setTextSize(2);
  display.print(round(h_min));
  display.setTextSize(1);
  display.print( "%" );
  display.setTextSize(2);
  display.print( "|" );
  display.setCursor(90,20);
  display.setTextSize(2);
  display.print(round(h_max));
  display.setTextSize(1);
  display.println( "%" );

  display.setTextSize(2);
  display.setCursor(2,40);  
  display.print(round(eCO2));
//  display.print(round(CO2));
  display.setTextSize(1);
  display.println( "co2" );
  display.setCursor(72,40);
  display.setTextSize(2);
  display.print(round(TVOC));
  display.setTextSize(1);
  display.println( "ppb" );

  display.display(); 
}


void readDHT() {
    h[0] = dht1.readHumidity();
    t[0] = dht1.readTemperature();
    hic[0] = dht1.computeHeatIndex(t[0], h[0], false);
    h[1] = dht2.readHumidity();
    t[1] = dht2.readTemperature();
    hic[1] = dht2.computeHeatIndex(t[1], h[1], false);
    h[2] = dht3.readHumidity();
    t[2] = dht3.readTemperature();
    hic[2] = dht3.computeHeatIndex(t[2], h[2], false);
    hm = 0;
    tm = 0;
    hicm= 0;
    
  // solo los 2 primeros sensore, el 3º está fallando
  for( int i = 0; i < 2; i ++ ) {
    if (isnan(h[i]) || isnan(t[i]) ) {
      h[i] = -hm;
      t[i] = -tm;
//      Serial.println(F("Failed to read from DHT sensor!"));
//      Serial.println(i);
    } else {
      hm += h[i];
      tm += t[i];
      hicm += hic[i];
    }
  }
  hm = hm / 2.0;
  tm = tm / 2.0;
  hicm = hicm / 2.0;
}

void readCCS() {
  if(ccs.available()){
    int r = ccs.readData();
    if(!r){
      float temp = ccs.calculateTemperature();
      eCO2 = ccs.geteCO2();
      TVOC = ccs.getTVOC();
    } else {
      delay(1);
    }
  } else {
//    Serial.println("CSS not available");
    delay(1);
  }
}
