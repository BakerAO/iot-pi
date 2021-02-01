#include <TinyGPS++.h>
#include <RH_RF95.h>

#define VBAT_PIN A7 // 12
#define INPUT_PIN A4 // 16
#define ENABLE_PIN A3 // 17
#define SENSOR_PIN A1 // 15
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
String deviceId = "10007";
TinyGPSPlus gps;
float lat, lon, alt;
uint8_t sats;
uint32_t hdop, startTime, sendTime;

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

  Serial1.begin(9600);
//  Serial.begin(115200);
}

void loop() {
  gpsWaitFix();
  readAndSend();
}


void gpsWaitFix() {
  uint8_t GPSchar;
  startTime = millis();
  while (1) {
    if (Serial1.available() > 0) {
      GPSchar = Serial1.read();
      gps.encode(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
      sendTime = millis();
      break;
    }
  }
}


void readAndSend() {
  String reading = "";
  float tempHDOP;
  int timeLength = sendTime - startTime;
  lat = gps.location.lat();
  lon = gps.location.lng();
  alt = gps.altitude.meters();
  sats = gps.satellites.value();
  hdop = gps.hdop.value();
  tempHDOP = ((float) hdop / 100);

  reading += "{ \"device_id\": " + deviceId;
  reading += ", \"battery\": " + readBattery();
  reading += ", \"latitude\": " + String(lat, 8);
  reading += ", \"longitude\": " + String(lon, 8);
  reading += ", \"satellites\": " + String(sats);
  reading += ", \"hdop\": " + String(tempHDOP, 2);
  reading += ", \"altitude\": \"" + String(alt, 1) + "\" }";

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
