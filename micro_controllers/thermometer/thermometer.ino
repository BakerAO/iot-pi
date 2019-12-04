#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastTrans;

String readSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String reading = "t-" + String(t);
  reading += ", h-" + String(h);
  return reading;
}

void setup() {
  dht.begin();
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (millis() > lastTrans + 60000) {
    String command = "AT+SEND=0,16,";
    String message = readSensor();
    command += message;
    Serial.println(command);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    lastTrans = millis();
  }
}
