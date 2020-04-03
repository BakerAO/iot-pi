#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include <RH_RF95.h>

#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5
#define TFT_RST -1
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(100);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_WHITE);
  tft.setTextSize(5);
  tft.println("Innov8 IoT");
  for (int i = 50; i < 56; i++) {
    // xStart, yStart, xEnd, yEnd, color
    tft.drawLine(0, i, 500, i, HX8357_WHITE);
  }
  tft.setTextSize(3);
  tft.println("");
}
 
void loop() {
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      String message = (char*)buf;
      displayText(message);
    }
  }
  delay(1000);
}

void displayText(String message) {
  for (int i = 100; i < 250; i++) {
    tft.drawLine(0, i, 500, i, HX8357_BLACK);
  }
  tft.setCursor(0, 100);
  tft.println(message);
}
