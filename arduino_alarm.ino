#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init(); // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("Kolya Pidr!");
  //lcd.setCursor(1,1);
  //lcd.print("Ywrobot Arduino!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
