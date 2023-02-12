#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

const int RTC_IO = 4;
const int RTC_SCLK = 5;
const int RTC_CE = 2;

const int LED_PIN = 8;
const int BTN_1_PIN = 11;
const int BTN_2_PIN = 12;
const int POT_PIN = A0;

const int DELAY_PERIOD = 1000 / 5;

const int MINUTES_FRACTION = 1024 / 60;
const int HOURS_FRACTION = 1024 / 24;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

ThreeWire rtc_wire(RTC_IO, RTC_SCLK, RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(rtc_wire);

unsigned long previous_time;
int time_delta;
int delay_time;

int pot_value;
int btn_1_state;
int btn_2_state;

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

  // setup buttons
  pinMode(BTN_1_PIN, INPUT);
  pinMode(BTN_2_PIN, INPUT);

  // setup lcd
  lcd.init();
  lcd.backlight();

  // variables initialization
  status = regular;
  previous_time = millis();

  // rtc setup
  Serial.begin(9600);

  Serial.print("compiled: ");
  Serial.print(String(__DATE__) + " ");
  Serial.println(__TIME__);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

void loop() {
  lcd.setCursor(0, 0);
  pot_value = analogRead(POT_PIN);
  btn_1_state = digitalRead(BTN_1_PIN);
  btn_2_state = digitalRead(BTN_2_PIN);

  minutes = pot_value / MINUTES_FRACTION;
  minutes = (minutes == 60 ? minutes - 1: minutes);

  hours = pot_value / HOURS_FRACTION;
  hours = (hours == 24 ? hours - 1 : hours);

  if (btn_1_state == HIGH) {
    Serial.println("btn_1 - HIGH" + String(btn_1_state));
  } else {
    Serial.println("btn_1 - LOW" + String(btn_1_state));
  }
  
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

  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid()) {
    // Common Causes:
    //   1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
  }
  
  lcd.setCursor(0, 1);
  lcd.print(padTime(now.Hour()));
  lcd.setCursor(3, 1);
  lcd.print(padTime(now.Minute()));
  lcd.setCursor(6, 1);
  lcd.print(padTime(now.Second()));

  // delay handling
  time_delta = millis() - previous_time;
  delay_time = (time_delta < DELAY_PERIOD ? DELAY_PERIOD - time_delta : 0);
  delay(delay_time);
  previous_time = millis();
}

String padTime(int time) {
  return time > 9 ? String(time) : "0" + String(time);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
