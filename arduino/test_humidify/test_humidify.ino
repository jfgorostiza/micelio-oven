#define HUM_PIN 2

bool humState = false;  // estado del humidificador

void setup() {
  Serial.begin( 9600 );
  // put your setup code here, to run once:
  pinMode(HUM_PIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  humChange();
  Serial.println(humState);
  delay(5000);
  humChange();
  Serial.println(humState);
  delay(1000);
}

/**/
void humChange(){
  digitalWrite( HUM_PIN, HIGH );
  delay(40);
  digitalWrite( HUM_PIN, LOW );
  humState = !humState;
}
