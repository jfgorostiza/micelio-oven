/*
main - micelio 
Javi F Gorostiza
09/01/2024

*/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <SD.h>
#include "DHT.h"

#define OLED_CLK 13
#define OLED_MOSI 11
#define OLED_CS 10
#define OLED_DC 8

// Used for I2C or SPI
#define OLED_RESET -1

#define DHT11PIN 2     // Digital pin connected to the DHT sensor
#define DHT22PIN 4     // Digital pin connected to the DHT sensor
#define HEAT_PIN  5

const int chipSelect = 53; // 4 in UNO, 53 in MEGA, for datalogger SD

//Adafruit_SSD1305 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// hardware SPI - use 7Mhz (7000000UL) or lower because the screen is rated for 4MHz, or it will remain blank!
//Adafruit_SSD1305 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS, 7000000UL);

Adafruit_SSD1305 display(128, 64, &Wire, OLED_RESET);
DHT dht11(DHT11PIN, DHT11);
DHT dht22(DHT22PIN, DHT11);

float t11, h11, hic11;
float t22, h22, hic22;
float t_min = 24.0;
float t_max = 25.0; // 32

const int pinHeater = 6;
bool heatState = false;
const int pinWater = 7;
bool waterState = false;

Sd2Card card;
SdVolume volume;
SdFile root;

File dataFile;
String data_file_name;
String dataString = "";
bool card_present = true;

int time_btwn_write = 3000; // tiempo entre escrituras en la SD
long time_last_write = 0; // ultima vez que se escribió en la SD
void(* reset)(void) = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }  
  randomSeed(analogRead(A0));

// DHT sensors
  Serial.print("Initializing DHT11 a...");
  dht11.begin();
  Serial.println("DHT11 a initialized.");
  Serial.print("Initializing DHT11 b...");
  dht22.begin();
  Serial.println("DHT11 b initialized.");

// OLED screen
  if ( ! display.begin(0x3C) ) {
     Serial.println("Unable to initialize OLED");
     while (1) yield();
  }
  display.clearDisplay();
    // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("Micelio");
  display.display(); 

/*  SD card */
  Serial.print("Initializing SD card...");
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    delay(1000);
    reset();
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }


  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    card_present = false;
  }
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.println("card initialized.");
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  int rn = 0;
  do {
    data_file_name = "data" + String(rn) + ".log";
    rn++;
  }while(SD.exists( data_file_name ));
  
//  data_file_name = "test.log";
  Serial.println("Writing in file: " + data_file_name);
  dataFile = SD.open( data_file_name, FILE_WRITE );
  dataString =  "temperatura, humedad, coeficiente, calentandor, humidificador, timestamp";
  if( dataFile ) {
    dataFile.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println("error opening .log");
  }

/**/  
  pinMode( HEAT_PIN, OUTPUT );
  digitalWrite(pinHeater, LOW);
  delay(2000); // wait DHT sensors
  time_last_write = millis();
}

void loop() {
  dataString = "";
  t_min = map(analogRead(A0), 0, 1023, 15, 35);
  t_max = map(analogRead(A1), 0, 1023, 15, 35);
  t11 = dht11.readTemperature();
  t22 = dht22.readTemperature();

  h11 = dht11.readHumidity();
  h22 = dht22.readHumidity();

  if (isnan( h11 ) || isnan( t11 )) {
    Serial.println(F("Failed to read from DHT11 sensor!"));
//    return;
  }
  if (isnan( h22 ) || isnan( t22 )) {
    Serial.println(F("Failed to read from DHT22 sensor!"));
//    return;
  }
  hic11 = dht11.computeHeatIndex( t11, h11, false );
  hic22 = dht11.computeHeatIndex( t22, h22, false );
  float t = (t11 + t22) / 2.0;
  float h = (h11 + h22) / 2.0;
  float hic = (hic11 + hic22) / 2.0;
  dataString += String( t ) + "," + String( h )+ "," + String( hic ) + ",";
//  dataString += String( t22 ) + "," + String( h22 )+ "," + String( hic22 );
  dataString += String(heatState) + "," + String(waterState) + ",";
  dataString += String(millis());
  /*
  Serial.print( t11); Serial.print( ",");
  Serial.print( h11);Serial.print( ",");
  Serial.println( hic11);

  Serial.print( t22); Serial.print( ",");
  Serial.print( h22);Serial.print( ",");
  Serial.println( hic22);
/**/
if(millis() - time_last_write > time_btwn_write) {
  dataFile = SD.open( data_file_name, FILE_WRITE );

  if( dataFile ) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("Write data in SD - OK!");
  }
  else {
    Serial.println("error opening .log");
  }
  time_last_write = millis();
}
/*  */
  Serial.println(dataString);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.print(t);display.println("C");
  display.setCursor(10,40);
  display.print(h);display.println("%");
  display.setTextSize(1);
  display.setCursor(100,10);
  display.println("min:"); 
  display.setCursor(85,20);
  display.println(t_min + String("C"));
  display.setCursor(100,35);
  display.println("max:"); 
  display.setCursor(85,45);
  display.println(t_max + String("C"));
  display.display(); // show splashscreen

  if( t11 < t_min ) { // Enciende el calentador
    Serial.println("COLD");
    heatState = true;
    digitalWrite( HEAT_PIN, HIGH );
  }
  if( t11 > t_max ) { // Apaga el calentador
    Serial.println("HOT");
    heatState = false;
    digitalWrite( HEAT_PIN, LOW );
  }
  delay(100);
}

