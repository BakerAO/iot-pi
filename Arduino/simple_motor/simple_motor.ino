#include <RH_RF95.h>
#include <TinyGPS++.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
#define VBAT_PIN A7 // 9
#define ENABLE_PIN A3 // 17

RH_RF95 rf95(RFM95_CS, RFM95_INT);
TinyGPSPlus gps;

String deviceId = "10003";
String valveStatus = "open";
float lat, lng, alt, tempHDOP;
uint32_t hdop, sats, lastTrans = 0;

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

  pinMode(ENABLE_PIN, OUTPUT);
  Serial1.begin(9600);
//  Serial.begin(115200);
  delay(2000);
  readGPS();
}

void loop() {
  readAndSend();
  receiveMessage();
  delay(1000);
}

void readAndSend() {
  String reading = readSensors();
  if (reading.length() > 0) {
    sendMessage(reading);
//    Serial.println(reading);
  }
}

String readSensors() {
  String reading = "";
  if ((millis() - lastTrans) > 10000) {
    readGPS();

    reading += "{ \"type\": \"simple_motor\"";
    reading += ", \"device_id\": " + deviceId;
    reading += ", \"battery\": " + readBattery();
    reading += ", \"valve_status\": \"" + valveStatus + "\"";
    reading += ", \"latitude\": " + String(lat, 8);
    reading += ", \"longitude\": " + String(lng, 8);
    reading += ", \"altitude\": " + String(alt, 1);
    reading += ", \"satellites\": " + String(sats);
    reading += ", \"hdop\": " + String(tempHDOP, 2);
    reading += " }";

    lastTrans = millis();
    return reading;
  }

  return "";
}

void readGPS() {
  uint8_t GPSchar;
  while (1) {
    if (Serial1.available() > 0) {
      GPSchar = Serial1.read();
      gps.encode(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
//    if (gps.hdop.isUpdated()) {
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

void sendMessage(String message) {
  int n = message.length() + 1;
  char radiopacket[n];
  strcpy(radiopacket, message.c_str());
  delay(10);
  rf95.send((uint8_t *)radiopacket, n);
  delay(10);
  rf95.waitPacketSent();
}

void receiveMessage() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      String message = (char*)buf;
      processMessage(message);
    }
  }
}

void processMessage(String message) {
  if (message.indexOf(deviceId + "-SHUT_OFF") == 0) {
    activateMotor();
  } else if (message.indexOf(deviceId + "-OPEN") == 0) {
    deactivateMotor();
  }
}

void activateMotor() {
  digitalWrite(ENABLE_PIN, HIGH);
  valveStatus = "closed";
}

void deactivateMotor() {
  digitalWrite(ENABLE_PIN, LOW);
  valveStatus = "open";
}

String readBattery() {
  float vbat = analogRead(VBAT_PIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  return String(vbat);
}
