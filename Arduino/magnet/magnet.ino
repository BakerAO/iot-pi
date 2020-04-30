#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

unsigned long lastTrans;
String deviceId = "10003";
int digitalInPin = 12;
int ledPin = 13;

String readSensor() {
  boolean stat = digitalRead(digitalInPin);
  if (stat) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }

  String reading = "{ ";
  reading += "\"device_id\": " + deviceId;
  reading += ", \"magnet\": " + String(stat);
  reading += " }";
  return reading;
}

void sendMessage(String message) {
  int n = message.length() + 1;
  char radiopacket[n];
  strcpy(radiopacket, message.c_str());
  delay(10);
  rf95.send((uint8_t *)radiopacket, n);
  delay(10);
  rf95.waitPacketSent();

//  Serial.println(radiopacket);
}

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);

  pinMode(digitalInPin,INPUT);
  pinMode(ledPin,OUTPUT);

//  Serial.begin(115200);
}

void loop() {
  if (millis() > lastTrans + 3000) {
    String message = readSensor();
    sendMessage(message);
    lastTrans = millis();
  }
}
