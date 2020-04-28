const int digitalInPin = 12;
const int ledPin = 13;

void setup() {
  pinMode(digitalInPin,INPUT);
  pinMode(ledPin,OUTPUT);
}

void loop() {
  boolean stat = digitalRead(digitalInPin);
  if(stat == HIGH) {
    digitalWrite(ledPin,LOW);
  }
  else {
    digitalWrite(ledPin,HIGH);
  }
}
