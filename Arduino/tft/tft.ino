#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5
#define TFT_RST -1

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.begin();
  tft.setRotation(3);
}


void loop(void) {
//  for(uint8_t rotation=0; rotation<4; rotation++) {
//    tft.setRotation(rotation);
    testText();
    delay(10000);
//    testLines(HX8357_YELLOW);
//    delay(1000);
//  }
}

void testText() {
  tft.fillScreen(HX8357_BLACK);
//  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(HX8357_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(HX8357_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(HX8357_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  
  tft.setTextColor(HX8357_WHITE);
  tft.println(F("Alic"));
  tft.drawLine(0, 150, 150, 150, HX8357_YELLOW);
//  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(HX8357_BLACK);
  
  x1 = y1 = 0;
  y2    = h - 1;
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
}

//unsigned long testFastLines(uint16_t color1, uint16_t color2) {
//  unsigned long start;
//  int           x, y, w = tft.width(), h = tft.height();
//
//  tft.fillScreen(HX8357_BLACK);
//  start = micros();
//  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
//  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);
//
//  return micros() - start;
//}

//unsigned long testRects(uint16_t color) {
//  unsigned long start;
//  int           n, i, i2,
//                cx = tft.width()  / 2,
//                cy = tft.height() / 2;
//
//  tft.fillScreen(HX8357_BLACK);
//  n     = min(tft.width(), tft.height());
//  start = micros();
//  for(i=2; i<n; i+=6) {
//    i2 = i / 2;
//    tft.drawRect(cx-i2, cy-i2, i, i, color);
//  }
//
//  return micros() - start;
//}

//unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
//  unsigned long start, t = 0;
//  int           n, i, i2,
//                cx = tft.width()  / 2 - 1,
//                cy = tft.height() / 2 - 1;
//
//  tft.fillScreen(HX8357_BLACK);
//  n = min(tft.width(), tft.height());
//  for(i=n; i>0; i-=6) {
//    i2    = i / 2;
//    start = micros();
//    tft.fillRect(cx-i2, cy-i2, i, i, color1);
//    t    += micros() - start;
//    // Outlines are not included in timing results
//    tft.drawRect(cx-i2, cy-i2, i, i, color2);
//  }
//
//  return t;
//}

//unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
//  unsigned long start;
//  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;
//
//  tft.fillScreen(HX8357_BLACK);
//  start = micros();
//  for(x=radius; x<w; x+=r2) {
//    for(y=radius; y<h; y+=r2) {
//      tft.fillCircle(x, y, radius, color);
//    }
//  }
//
//  return micros() - start;
//}
//
//unsigned long testCircles(uint8_t radius, uint16_t color) {
//  unsigned long start;
//  int           x, y, r2 = radius * 2,
//                w = tft.width()  + radius,
//                h = tft.height() + radius;
//
//  // Screen is not cleared for this one -- this is
//  // intentional and does not affect the reported time.
//  start = micros();
//  for(x=0; x<w; x+=r2) {
//    for(y=0; y<h; y+=r2) {
//      tft.drawCircle(x, y, radius, color);
//    }
//  }
//
//  return micros() - start;
//}
//
//unsigned long testTriangles() {
//  unsigned long start;
//  int           n, i, cx = tft.width()  / 2 - 1,
//                      cy = tft.height() / 2 - 1;
//
//  tft.fillScreen(HX8357_BLACK);
//  n     = min(cx, cy);
//  start = micros();
//  for(i=0; i<n; i+=5) {
//    tft.drawTriangle(
//      cx    , cy - i, // peak
//      cx - i, cy + i, // bottom left
//      cx + i, cy + i, // bottom right
//      tft.color565(200, 20, i));
//  }
//
//  return micros() - start;
//}
//
//unsigned long testFilledTriangles() {
//  unsigned long start, t = 0;
//  int           i, cx = tft.width()  / 2 - 1,
//                   cy = tft.height() / 2 - 1;
//
//  tft.fillScreen(HX8357_BLACK);
//  start = micros();
//  for(i=min(cx,cy); i>10; i-=5) {
//    start = micros();
//    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
//      tft.color565(0, i, i));
//    t += micros() - start;
//    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
//      tft.color565(i, i, 0));
//  }
//
//  return t;
//}
//
//unsigned long testRoundRects() {
//  unsigned long start;
//  int           w, i, i2,
//                cx = tft.width()  / 2 ,
//                cy = tft.height() / 2 ;
//
//  tft.fillScreen(HX8357_BLACK);
//  w     = min(tft.width(), tft.height());
//  start = micros();
//  for(i=0; i<w; i+=8) {
//    i2 = i / 2 - 2;
//    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 100, 100));
//  }
//
//  return micros() - start;
//}
//
//unsigned long testFilledRoundRects() {
//  unsigned long start;
//  int           i, i2,
//                cx = tft.width()  / 2 + 10,
//                cy = tft.height() / 2 + 10;
//
//  tft.fillScreen(HX8357_BLACK);
//  start = micros();
//  for(i=min(tft.width(), tft.height()) - 20; i>25; i-=6) {
//    i2 = i / 2;
//    tft.fillRoundRect(cx-i2, cy-i2, i-20, i-20, i/8, tft.color565(100, i/2, 100));
//  }
//
//  return micros() - start;
//}
