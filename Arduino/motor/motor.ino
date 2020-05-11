#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int enablePin = 17; // A3
int inputPin1 = 11;
int inputPin2 = 12;
int motorSpeed = 0; // 0 to 255
String deviceId = "10005";

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

  pinMode(enablePin, OUTPUT);
  pinMode(inputPin1, OUTPUT);
  pinMode(inputPin2, OUTPUT);

  Serial.begin(115200);
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
  delay(1000);
}

void processMessage(String message) {
  Serial.println(message);
  if (message.indexOf(deviceId + "-ON") == 0) {
    activateMotor();
  } else if (message.indexOf(deviceId + "-OFF") == 0) {
    deactivateMotor();
  }
}

void activateMotor() {
  motorSpeed = 200;
  digitalWrite(inputPin1, HIGH);
  digitalWrite(inputPin2, LOW);
  analogWrite(enablePin, motorSpeed);
}

void reverseMotor() {
  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, HIGH);
}

void deactivateMotor() {
  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, LOW);
}

void loop() {
  receiveMessage();
}
