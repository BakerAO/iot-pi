#include <RH_RF95.h>

#define VBATPIN A7
#define LED_PIN 13
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

byte sensorPin = 6;
int inputPin1 = 11;
int inputPin2 = 12;
int enablePin = 17; // A3
int motorSpeed = 0; // 0 to 255
String valveStatus = "open";
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long lastTrans;
// The hall-effect flow sensor outputs approximately 
// 4.5 pulses per second per litre/minute of flow
float calibrationFactor = 4.5;
String deviceId = "10004";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
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
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  lastTrans = 0;

  // Trigger on a FALLING state change (transition from HIGH state to LOW state)
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  // Serial.begin(115200);
}

void loop() {
  String reading = readSensor();
  if (reading.length() > 0) {
    sendMessage(reading);
  }
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
  motorSpeed = 200;
  digitalWrite(inputPin1, HIGH);
  digitalWrite(inputPin2, LOW);
  analogWrite(enablePin, motorSpeed);
  valveStatus = "closed";
}

void reverseMotor() {
  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, HIGH);
}

void deactivateMotor() {
  motorSpeed = 0;
  digitalWrite(inputPin1, LOW);
  digitalWrite(inputPin2, LOW);
  analogWrite(enablePin, motorSpeed);
  valveStatus = "open";
}

String readSensor() {
  String reading = "";
  if ((millis() - lastTrans) > 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - lastTrans)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    lastTrans = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    reading += "{ \"device_id\": " + deviceId;
    reading += ", \"battery\": " + readBattery();
    reading += ", \"flow_rate\": " + String(flowRate); // Litres per minute
    reading += ", \"total_output\": " + String(totalMilliLitres/1000); // Litres
    reading += ", \"valve_status\": \"" + valveStatus + "\" }";

    pulseCount = 0;
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  }

  if (flowRate > 0) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    return reading;
  } else {
    return "";
  }
}

// Insterrupt Service Routine
void pulseCounter() {
  pulseCount++;
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
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  return String(vbat);
}
