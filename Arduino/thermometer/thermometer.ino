#include <RH_RF95.h>
#include <DHT.h>

#define VBAT_PIN A7 // 12
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define DHT_PIN 12
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastTrans;
String deviceId = "10001";

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
  if (millis() > lastTrans + 30000) {
    String message = readSensor();
    sendMessage(message);
    lastTrans = millis();
  }
}

String readSensor() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String reading = "";
  reading += "{ \"device_id\": " + deviceId;
  reading += ", \"battery\": " + readBattery();
  reading += ", \"temperature\": " + String(temperature);
  reading += ", \"humidity\": " + String(humidity) + " }";
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
}

String readBattery() {
  float vbat = analogRead(VBAT_PIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  return String(vbat);
}
