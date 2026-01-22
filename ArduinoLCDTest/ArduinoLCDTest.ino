#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address. 0x27 is the most common.
// If this doesn't work, try 0x3F.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  // Set the cursor to the first line (line 0)
  lcd.setCursor(0, 0);
  lcd.print("Cho Cho Aung");
  
  // Set the cursor to the second line (line 1)
  lcd.setCursor(0, 1);
  lcd.print("Gatone K");
}

void loop() {
  // Nothing to do here
}
