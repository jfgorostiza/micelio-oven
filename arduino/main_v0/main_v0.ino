#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include "Adafruit_CCS811.h"
#include "DHT.h"


#define HUM_PIN 2
#define DHT11_1 3
#define DHT11_2 4
#define DHT22_PIN 5
#define CO2 6

#define HEAT_PIN  53
#define FAN_PIN 51

#define HUM_STATE_PIN 9

// Used for I2C or SPI
#define OLED_RESET 9//13
#define OLED_DC 10//8
#define OLED_MOSI 11//11
#define OLED_CS 12//10
#define OLED_CLK 13//13

// software SPI
// Este display es incompatible con el sensor CCS811:
Adafruit_SSD1305 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//Adafruit_SSD1305 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS, 7000000UL);

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

void setup() {
  Serial.begin( 9600 );
  while( !Serial );
  Serial.println("OVEN-2 MAIN");
  
  /* Heater */
  Serial.println("Heater OK");
  pinMode( HEAT_PIN, OUTPUT );
  digitalWrite( HEAT_PIN, HIGH );
  
  /* Heater */
  Serial.println("Fan OK");
  pinMode( FAN_PIN, OUTPUT );
  digitalWrite( FAN_PIN, HIGH );

  /* Humidifator */
  Serial.println("Humidificator OK");
  pinMode( HUM_PIN, OUTPUT );
  pinMode( HUM_STATE_PIN, INPUT );
  bool s = humChange();
  Serial.print("HUM state = "); Serial.println(s);
  delay(500);
  humChange();
  Serial.print("HUM state = "); Serial.println(s);
/**/
  Serial.println("Display...");
  if ( ! display.begin(0x3C) ) {
     Serial.println("Unable to initialize OLED");
  } else Serial.println("OK");
  display.display(); // show splashscreen
  delay(1000);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Micelio-v2");
  display.display(); 

/**/
  Serial.println("Sensor CSS...");
  if(!ccs.begin()){
    Serial.println("Failed to start CCS811 sensor! Please check your wiring.");
  } else Serial.println("OK");
//  ccs.SWReset();
/**/
  Serial.println( "DHT_1 - begin" );
  dht1.begin();
  Serial.println( "DHT_2 - begin" );
  dht2.begin();
  Serial.println( "DHT_3 - begin" );
  dht3.begin();
  readDHT();
/**/
  Serial.println("CCS811 temp offset:");
  while(!ccs.available());
  ccs.setDriveMode( CCS811_DRIVE_MODE_250MS ); // 0x04
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - tm);
  Serial.println(temp);
//  testActuators();
/**/
}

void loop() {
  t_min = map(analogRead(A1), 0, 1023, 15, 35);
  t_max = map(analogRead(A0), 0, 1023, 15, 35);
  h_min = map(analogRead(A2), 0, 1023, 50, 100);
  h_max = map(analogRead(A3), 0, 1023, 50, 100);
  printThresValues();
  humState = digitalRead( HUM_STATE_PIN );
  Serial.print( "HUM_STATE = " );Serial.println(humState);
  readDHT();
//  Serial.print("tm = "); Serial.print(tm); Serial.print(" hm = "); Serial.println( hm );
  printDHTvalues();
//  readCCS();

  if( tm < t_min ) { // Enciende el calentador
    Serial.println("Hace FRIO");
    heatState = true;
    digitalWrite( HEAT_PIN, LOW );
//    digitalWrite( FAN_PIN, HIGH ); // apaga fan
  }
  if( tm > t_max ) { // Apaga el calentador
    Serial.println("Hace CALOR");
    heatState = false;
    digitalWrite( HEAT_PIN, HIGH );
//    digitalWrite( FAN_PIN, LOW ); // enciende fan
  }

  if( hm < h_min ) { // Enciende el humidificador
    Serial.println("Estamos SECOS");
    humChange();
    digitalWrite( FAN_PIN, HIGH );  // apaga fan
  }
  if( hm > h_max ) { // Apaga humidificador
    Serial.println("Estamos HUMEDOS");
    humChange();
    digitalWrite( FAN_PIN, LOW ); // enciende fan
  }

  if( Serial.available() ){
//    String cmd = Serial.readStringUntil("\n");
    String cmd = Serial.readString();
//    Serial.println(cmd);
    if( cmd == "GET_DATA" ){
      sendData();
    }
    else if( cmd.startsWith("SEND_DATA:") ){
      String data = cmd.substring(10);
      parseData(data);   
    }
    else if( cmd.startsWith("DO_ACTION:") ){
      String action = cmd.substring(10);
      doAction(action);
    }
  }
  delay(2000);
}

void printThresValues() {
  Serial.print("t_min = "); Serial.print(t_min);
  Serial.print("  t_max = "); Serial.print(t_max);
  Serial.print("  h_min = "); Serial.print(h_min);
  Serial.print("  h_max = "); Serial.println(h_max);
}

void testActuators() {
  Serial.println( "HEAT_PIN_1 to LOW" );
  digitalWrite( HEAT_PIN, LOW );
  delay(1000);
  Serial.println( "HEAT_PIN_1 to HIGH" );
  digitalWrite( HEAT_PIN, HIGH );
  delay(1000);
  Serial.println( "FAN to LOW" );
  digitalWrite( FAN_PIN, LOW );
  delay(1000);
  Serial.println( "FAN to HIGH" );
  digitalWrite( FAN_PIN, HIGH );
  delay(1000);
  Serial.println( "HUM change 1" );
  humChange();
  delay(1000);
  Serial.println( "HUM change 2" );
  humChange();
  delay(1000);
}
/**/
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

  // Check if any reads failed and exit early (to try again).
  for( int i = 0; i < 2; i ++ ) {
    if (isnan(h[i]) || isnan(t[i]) ) {
      Serial.println(F("Failed to read from DHT sensor!"));
      Serial.println(i);
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

void printDHTvalues() {
  for( int i = 0; i < 3; i ++ ) {

    Serial.print( "Humidity: " );
    Serial.print(h[i]);
    Serial.print( "%  Temperature: " );
    Serial.print(t[i]);
    Serial.print( "°C " );
    Serial.print( "Heat index: " );
    Serial.print(hic[i]);
    Serial.println( "°C " );
  }
  Serial.print( "Humidity media: " );
  Serial.print(hm);
  Serial.print( "%  Temperature media: " );
  Serial.print(tm);
  Serial.print( "°C " );
  Serial.print( "Heat index media: " );
  Serial.print(hicm);
  Serial.println( "°C " );

}
/**/
void readCCS() {
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
    } else {
      delay(1);
    }
  } else {
//    Serial.println("CSS not available");
    delay(1);
  }
}

/**/
bool humChange(){
  digitalWrite( HUM_PIN, HIGH );
  delay(40);
  digitalWrite( HUM_PIN, LOW );
  humState = digitalRead( HUM_STATE_PIN );
  return(humState);
}

void doAction( String action ){
//  humChange();
  Serial.println(action);
}

void parseData( String data ){
  int n = data.toInt();
  for(int i = 0; i <n; i++){
    humChange();
    delay(2000);  
  }
}

void sendData() {
  humChange();
  String str = "";
  int v = analogRead(A0);
  str ="25";
  Serial.println( v ); 
}
