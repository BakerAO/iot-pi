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
  dht.begin();
  Serial.begin(115200);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
}

void loop() {
  if (millis() > lastTrans + 5000) {
    String message = readSensor();
    int n = message.length() + 1;
    char radiopacket[n];
    strcpy(radiopacket, message.c_str());
    Serial.println(radiopacket);
    delay(10);
    rf95.send((uint8_t *)radiopacket, n);
    delay(10);
    rf95.waitPacketSent();
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
//    if (rf95.waitAvailableTimeout(1000)) { 
//      if (rf95.recv(buf, &len)) {
//        Serial.print("Got reply: ");
//        Serial.println((char*)buf);
//        Serial.print("RSSI: ");
//        Serial.println(rf95.lastRssi(), DEC);    
//      }
//    }
    lastTrans = millis();
  }
}
