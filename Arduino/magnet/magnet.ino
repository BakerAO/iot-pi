#include <RH_RF95.h>

#define INPUT_PIN 15 // A1
#define VBAT_PIN A7 // 12
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

String deviceId = "10006";
unsigned long lastReading = 0;
unsigned long lastTrans = 0;
boolean magnetStatus = 0;

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);

  pinMode(INPUT_PIN, INPUT_PULLUP);
//  Serial.begin(115200);
}

void loop() {
  if (millis() > lastReading + 2000) {
    readSensor();
    lastReading = millis();
  }
}

void readSensor() {
  boolean currentStatus = digitalRead(INPUT_PIN);
  if (magnetStatus != currentStatus || millis() - lastTrans > 60000) {
    magnetStatus = currentStatus;
    String reading = "";
    reading += "{ \"device_id\": " + deviceId;
    reading += ", \"battery\": " + readBattery();
    reading += ", \"magnet\": " + String(magnetStatus) + " }";
    sendMessage(reading);
    lastTrans = millis();
  }
}

void sendMessage(String message) {
  int n = message.length() + 1;
  char radiopacket[n];
  strcpy(radiopacket, message.c_str());
  delay(10);
  rf95.send((uint8_t *)radiopacket, n);
  delay(10);
  rf95.waitPacketSent();
}

String readBattery() {
  float vbat = analogRead(VBAT_PIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  return String(vbat);
}
