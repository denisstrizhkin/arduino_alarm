#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int LED_PIN = 8;
const int POT_PIN = A0;

const int DELAY_PERIOD = 1000 / 5;

const int MINUTES_FRACTION = 1024 / 60;
const int HOURS_FRACTION = 1024 / 24;

unsigned long previous_time;
int time_delta;
int delay_time;

int pot_value;

int minutes;
int hours;

int alarm_minutes;
int alarm_hours;

enum State { regular, set_time, alarm };

State status;

void setup() {
  // setup led
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // setup lcd
  lcd.init();
  lcd.backlight();

  status = regular;
  previous_time = millis();
}

void loop() {
  lcd.setCursor(0, 0);
  pot_value = analogRead(POT_PIN);

  minutes = pot_value / MINUTES_FRACTION;
  minutes = (minutes == 60 ? minutes - 1: minutes);

  hours = pot_value / HOURS_FRACTION;
  hours = (hours == 24 ? hours - 1 : hours);
  
  switch (status) {
    case regular:
        lcd.print("regular mode");
      break;
    case set_time:
        lcd.print("set_time mode");
      break;
    case alarm:
        lcd.print("alarm mode");
      break;
  }
  
  lcd.setCursor(0, 1);
  lcd.print(padTime(hours));
  lcd.setCursor(3, 1);
  lcd.print(padTime(minutes));

  // delay handling
  time_delta = millis() - previous_time;
  delay_time = (time_delta < DELAY_PERIOD ? DELAY_PERIOD - time_delta : 0);
  delay(delay_time);
  previous_time = millis();
}

String padTime(int time) {
  return time > 9 ? String(time) : "0" + String(time);
}
