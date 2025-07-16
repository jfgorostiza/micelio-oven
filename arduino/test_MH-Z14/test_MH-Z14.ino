// set interrupt pin numbers to recive outhput signal:
const int sensorPin = 6; // Pin 2 as the sensor pin
long top = 0;
long down = 0;
long top_time = 0;
long down_time = 0;

void setup() {
  Serial.begin(9600);
// set sensor pin as an input
  pinMode(sensorPin, INPUT);
// Read starting value for millis…
  while(digitalRead(sensorPin) == LOW) {;}
  down = millis();
}

void loop(){
// Check for high and low timing – low time * 2 = PPM
  while(digitalRead(sensorPin) == HIGH) {;}
  top = millis();
  down_time = top - down;
  while(digitalRead(sensorPin) == LOW) {;}
  down = millis();
  top_time = down - top;
// Validity check high+low = 1004 (small margin or error allowed -> 990)
  if(top_time + down_time > 990) {
    Serial.print("H: ");
    Serial.println(down_time,DEC);
    Serial.print("L: ");
    Serial.println(top_time,DEC);
    Serial.print("CO2: ");
    Serial.print(top_time * 2,DEC);
    Serial.println(" ppm");
  } else {
// Output in case of invalid check: value instead of 1004 ms
    Serial.print(" CK:");
    Serial.println(top_time+down_time, DEC);
  }
}