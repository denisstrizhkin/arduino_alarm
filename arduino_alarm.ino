#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

const int RTC_IO = 4;
const int RTC_SCLK = 5;
const int RTC_CE = 2;

const int LED_PIN = 8;
const int BTN_1_PIN = A1;
const int BTN_3_PIN = A2;
const int BTN_2_PIN = A3;
const int POT_PIN = A0;

const int DELAY_PERIOD = 1000 / 15;

const int MINUTES_FRACTION = 1024 / 60;
const int HOURS_FRACTION = 1024 / 24;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

ThreeWire rtc_wire(RTC_IO, RTC_SCLK, RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(rtc_wire);

unsigned long previous_time;
int time_delta;
int delay_time;

int led_value = LOW;
int pot_value;
int btn_1_state;
int btn_2_state;
int btn_3_state;

bool is_btn_1= false;
bool is_btn_1_available = true;
bool is_btn_2 = false;
bool is_btn_2_available = true;
bool is_btn_3 = false;
bool is_btn_3_available = true;

int minutes;
int hours;

int alarm_minutes = -1;
int alarm_hours = -1;

enum State { regular, set_time, alarm };
enum SetTimeType { hour, minute };

State status = regular;
SetTimeType set_time_type = hour;

void setup() {
  // setup led
  pinMode(LED_PIN, OUTPUT);

  // setup buttons for digital input
  //pinMode(BTN_1_PIN, INPUT);
  //pinMode(BTN_2_PIN, INPUT);

  // setup lcd
  lcd.init();
  lcd.backlight();

  // set initial time
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
  digitalWrite(LED_PIN, led_value);
  lcd.setCursor(0, 0);
  pot_value = analogRead(POT_PIN);
  btn_1_state = analogRead(BTN_1_PIN);
  btn_2_state = analogRead(BTN_2_PIN);
  btn_3_state = analogRead(BTN_3_PIN);

  if (status == set_time || status == alarm) {
    if (set_time_type == minute) {
      minutes = pot_value / MINUTES_FRACTION;
      minutes = (minutes == 60 ? minutes - 1: minutes);
    }

    if (set_time_type == hour) {
      hours = pot_value / HOURS_FRACTION;
      hours = (hours == 24 ? hours - 1 : hours);
    }
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid()) {
    // Common Causes:
    //   1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
  }

  if (btn_1_state == 0) {
    if (is_btn_1_available) {
      is_btn_1_available = false;
      Serial.println("btn 1 pressed");
      if (status == regular) {
        status = set_time;
      } else if (status == set_time) {
        status = regular;
        Rtc.SetDateTime(RtcDateTime(2002,1,1,hours,minutes,now.Second()));
        set_time_type = hour;
      }
    }
  } else {
    is_btn_1_available = true;
  }

  if (btn_3_state == 0) {
    if (is_btn_3_available) {
      is_btn_3_available = false;
      Serial.println("btn 3 pressed");
      if (status == regular) {
        status = alarm;
      } else if (status == alarm) {
        status = regular;
        alarm_hours = hours;
        alarm_minutes = minutes;
        set_time_type = hour;
      }
    }
  } else {
    is_btn_3_available = true;
  }

  if (btn_2_state == 0) {
    if (is_btn_2_available && (status == set_time || status == alarm)) {
      is_btn_2 = true;
      is_btn_2_available = false;
      set_time_type = (set_time_type == hour ? minute : hour);
      Serial.println("btn 2 pressed");
    }
  } else {
    is_btn_2_available = true;
  }
  
  switch (status) {
    case regular:
        lcd.print("regular mode  ");
      break;
    case set_time:
        lcd.print("set_time mode");
      break;
    case alarm:
        lcd.print("alarm mode    ");
      break;
  }

  if (now.Hour() == alarm_hours && now.Minute() == alarm_minutes) {
    led_value = HIGH;
  }
  
  lcd.setCursor(0, 1);
  lcd.print(padTime(now.Hour()));
  lcd.setCursor(3, 1);
  lcd.print(padTime(now.Minute()));
  lcd.setCursor(6, 1);
  lcd.print(padTime(now.Second()));

  
  if ((status == set_time || status == alarm)) {
    lcd.setCursor(10, 1);
    lcd.print(padTime(hours));
    lcd.setCursor(13, 1);
    lcd.print(padTime(minutes));
  } else {
    lcd.setCursor(10,1);
    lcd.print("      ");
  }

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
