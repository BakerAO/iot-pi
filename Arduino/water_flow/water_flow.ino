#include <RH_RF95.h>

#define VBAT_PIN A7 // 12
#define INPUT_PIN 6
#define ENABLE_PIN A3 // 17
#define SENSOR_PIN 11
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

String deviceId = "10004";
String valveStatus = "open";
volatile byte pulseCount = 0;
float flowRate = 0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long lastTrans = 0;
unsigned long lastReading = 0;
// The hall-effect flow sensor outputs approximately
// 4.5 pulses per second per litre/minute of flow
float calibrationFactor = 4.5;

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

  pinMode(INPUT_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(SENSOR_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
//  Serial.begin(115200);
}

void loop() {
  readAndSend(false);
  receiveMessage();
  delay(1000);
}

void readAndSend(bool forceSend) {
  String reading = readSensor(forceSend);
  if (reading.length() > 0) {
    sendMessage(reading);
  }
}

String readSensor(bool forceSend) {
  String reading = "";
  if ((millis() - lastReading) > 1000 || forceSend) {
    detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - lastReading)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    lastReading = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    if (totalMilliLitres > 100000) {
      totalMilliLitres = 0;
    }
    totalMilliLitres += flowMilliLitres;

    reading += "{ \"device_id\": " + deviceId;
    reading += ", \"battery\": " + readBattery();
    reading += ", \"flow_rate\": " + String(flowRate); // Litres per minute
    reading += ", \"total_output\": " + String(totalMilliLitres/1000); // Litres
    reading += ", \"valve_status\": \"" + valveStatus + "\" }";

    pulseCount = 0;
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
    lastTrans++;
  }

  if (forceSend || flowRate > 0 || lastTrans > 29 || (valveStatus == "closed" && lastTrans > 4)) {
    lastTrans = 0;
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
  analogWrite(ENABLE_PIN, 255);
  digitalWrite(INPUT_PIN, HIGH);
  valveStatus = "closed";
}

void deactivateMotor() {
  analogWrite(ENABLE_PIN, 0);
  digitalWrite(INPUT_PIN, LOW);
  valveStatus = "open";
  readAndSend(true);
}

String readBattery() {
  float vbat = analogRead(VBAT_PIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  return String(vbat);
}
