#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int LED_PIN = 8;
const int POT_PIN = A0;

const int DELAY_PERIOD = 1000 / 5;

const int MINUTES_FRACTION = 1024 / 60;
const int HOURS_FRACTION = 1024 / 12;

unsigned long previous_time;
int time_delta;
int delay_time;

int cursorPos;
int pot_value;

int minutes;
int hours;

void setup() {
  // setup led
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // setup lcd
  lcd.init();
  lcd.backlight();
  cursorPos = 0;

  previous_time = millis();
}

void loop() {
  /*
  lcd.clear();
  lcd.setCursor(cursorPos, 0);
  lcd.print("Test!");
  cursorPos = (cursorPos == 15 ? 0 : cursorPos + 1);
  */
  pot_value = analogRead(POT_PIN);

  minutes = pot_value / MINUTES_FRACTION;
  minutes = (minutes == 60 ? minutes: minutes + 1);

  hours = pot_value / HOURS_FRACTION;
  hours = (hours == 12 ? hours : hours + 1);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(hours);

  // delay handling
  time_delta = millis() - previous_time;
  delay_time = (time_delta < DELAY_PERIOD ? DELAY_PERIOD - time_delta : 0);
  delay(delay_time);
  previous_time = millis();
}
