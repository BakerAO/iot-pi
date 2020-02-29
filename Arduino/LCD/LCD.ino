#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display         ";
int time = 2500;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
    lcd.setCursor(0, 0);  // set the cursor to column 15, line 0
    lcd.print("Hello");

    lcd.setCursor(0, 1);  // set the cursor to column 15, line 1
    lcd.print("World");  
    delay(time);
}
