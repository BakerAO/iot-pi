#include <RH_RF95.h>
#include <TinyGPS++.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define VBAT_PIN A7 // 9
#define ENABLE_PIN A3 // 17
#define SENSOR_PIN A4 // 18
#define TEMPERATURE_PIN A5 // 19

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
TinyGPSPlus gps;
OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature temperatureSensor(&oneWire);

String deviceId = "10005";
String valveStatus = "open";
volatile byte pulseCount = 0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0, lastTrans = 0, lastReading = 0;
// The hall-effect flow sensor outputs approximately
// 4.5 pulses per second per litre/minute of flow
float flowRate = 0, calibrationFactor = 4.5, lat, lng, alt, tempHDOP;
uint32_t hdop, sats;

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
  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(SENSOR_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
  temperatureSensor.begin();
  Serial1.begin(9600);
//  Serial.begin(115200);
  delay(2000);
//  Serial.println("Start");
  readGPS();
}

void loop() {
//  Serial.println("Loop");
  readAndSend();
//  receiveMessage();
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
  if ((millis() - lastReading) > 10000) {
    readFlow();
    readGPS();

    reading += "{ \"device_id\": " + deviceId;
    reading += ", \"battery\": " + readBattery();
    reading += ", \"temperature\": " + readTemperature();
    reading += ", \"flow_rate\": " + String(flowRate); // Litres per minute
    reading += ", \"total_output\": " + String(totalMilliLitres/1000); // Litres
    reading += ", \"valve_status\": \"" + valveStatus + "\"";
    reading += ", \"latitude\": " + String(lat, 8);
    reading += ", \"longitude\": " + String(lng, 8);
    reading += ", \"altitude\": " + String(alt, 1);
    reading += ", \"satellites\": " + String(sats);
    reading += ", \"hdop\": " + String(tempHDOP, 2);
    reading += " }";

    lastTrans++;
  }

  if (flowRate > 0 || lastTrans > 2 || (valveStatus == "closed" && lastTrans > 1)) {
    lastTrans = 0;
    return reading;
  } else {
    return "";
  }
}

void readFlow() {
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

  pulseCount = 0;
  // Enable the interrupt again now that we've finished sending output
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
}

void readGPS() {
  uint8_t GPSchar;
  float startTime = millis();
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

String readTemperature() {
  temperatureSensor.requestTemperatures();
  return String(temperatureSensor.getTempCByIndex(0));
}

// Interrupt Service Routine
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
