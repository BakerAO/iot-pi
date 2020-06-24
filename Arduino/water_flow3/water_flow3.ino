#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int inputPin = 11;
String deviceId = "20001";

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

  pinMode(inputPin, OUTPUT);
}

void loop() {
  receiveMessage();
  delay(1000);
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
  digitalWrite(inputPin, HIGH);
}

void deactivateMotor() {
  digitalWrite(inputPin, LOW);
}
