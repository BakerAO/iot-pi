#include <RH_RF95.h>
#include <Adafruit_GPS.h>

#define VBAT_PIN A7 // 12
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
String deviceId = "10007";
Adafruit_GPS gps(&Serial1);

long lat, lon;
unsigned long fix_age, time, date, speed, course;
unsigned long chars;
unsigned short sentences, failed_checksum;

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

  gps.begin(9600);

  // Serial1.begin(9600);
  Serial.begin(115200);
}

void loop() {
  readAndSend();
  // delay(10000);
}

void readAndSend() {
  String reading = readSensor();
  if (reading.length() > 0) {
    sendMessage(reading);
    Serial.println(reading);
  }
}

String readSensor() {
  String reading = "";
  if (gps.newNMEAreceived()) {
      gps.parse(gps.lastNMEA());
      lat = gps.latitudeDegrees;
      reading += "{ \"device_id\": " + deviceId;
      reading += ", \"battery\": " + readBattery();
      reading += ", \"lat\": " + String(lat); 
      reading += ", \"lng\": " + String(lon) + " }";
  }

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
