#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastTrans;

String readSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String reading = "temp-" + String(t);
  reading += ", hum-" + String(h);
  return reading;
}

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  dht.begin();
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
//  Serial.begin(115200);
}

void loop() {
  if (millis() > lastTrans + 5000) {
    String message = readSensor();
    int n = message.length() + 1;
    char radiopacket[n];
    strcpy(radiopacket, message.c_str());
    delay(10);
    rf95.send((uint8_t *)radiopacket, n);
    delay(10);
    rf95.waitPacketSent();
    lastTrans = millis();
//    Serial.println(radiopacket);
  }
}
