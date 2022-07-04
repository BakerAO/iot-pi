#include <RH_RF95.h>
#include <TinyGPS++.h>

#define VBAT_PIN A7 // 12
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
TinyGPSPlus gps;

String deviceId = "10003";
float lat, lng, alt, tempHDOP;
uint32_t hdop, sats, lastTrans;

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

  lastTrans = 0;
  Serial1.begin(9600);
//  Serial.begin(115200);
  delay(2000);
  readGPS();
}

void loop() {
  if (lastTrans + 10000 < millis()) {
    readGPS();
    readAndSend();
    lastTrans = millis();
  }
  delay(1000);
}

void readGPS() {
  uint8_t GPSchar;
  while (1) {
    if (Serial1.available() > 0) {
      GPSchar = Serial1.read();
      gps.encode(GPSchar);
    }

//    if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
    if (gps.hdop.isUpdated()) {
      lat = gps.location.lat();
      lng = gps.location.lng();
      alt = gps.altitude.meters();
      sats = gps.satellites.value();
      hdop = gps.hdop.value();
      tempHDOP = ((float) hdop / 100);
      break;
    }
  }
}

void readAndSend() {
  String reading = "";

  reading += "{ \"device_id\": " + deviceId;
  reading += ", \"battery\": " + readBattery();
  reading += ", \"latitude\": " + String(lat, 8);
  reading += ", \"longitude\": " + String(lng, 8);
  reading += ", \"altitude\": " + String(alt, 2);
  reading += ", \"satellites\": " + String(sats);
  reading += ", \"hdop\": " + String(tempHDOP, 2) + " }";

//  Serial.println(reading);
  sendMessage(reading);
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
