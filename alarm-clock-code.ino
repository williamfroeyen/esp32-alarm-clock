// ALARM CLOCK - Finished 09.12.2024 - Made by William Frøyen
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <math.h>
 
RTC_DS3231 rtc;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

// DECLARE AND SET VARIABLES
int buttonPin1 = 15;
int buttonPin2 = 25;
int buttonPin3 = 26;
int buttonPin4 = 27;
int dhtPin = 23;
int buzzerPin = 5;
int setMinutes = 0;
int setHours = 0;
int setAlarmMinutes = 61;
int setAlarmHours = 25;
int setClockHours = 0;
int setClockMinutes = 0;
int setYear = 2020;
int setMonth = 1;
int setDay = 1;
int i = 0;
int buzzerState = LOW;
int burnInDisplace = 0;
int lastMinute = 0;
int currentMinute = 0;
bool displayStateOn = true;
unsigned long lastReadTime1 = 0;
unsigned long lastReadTime2 = 0;
unsigned long lastReadTime3 = 0;
const long interval1s = 1000;
const long interval2s = 2000;
const long interval30s = 30000;
bool alarmStopped = false;
int hydr = 0;
int temp = 0;
String hour;
String minute;
String day;
String month;
String tempString;
String hydrString;
char days[7][5] = {"sun.", "mon.", "tue.", "wed.", "thu.", "fri.", "sat."};
int alarmToneCount = 20;

#define dhtType DHT11
DHT dht(dhtPin, dhtType);

void setup() {
  Serial.begin(115200);
  // START DISPLAY AND REAL-TIME CLOCK
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (! rtc.begin()) {
      Serial.println("RTC not found.");
      while (1);
    }

  // SET INPUT AND OUTPUT PINS
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  // INTRO DISPLAY
  dht.begin();
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(32, 25);
  display.println("ALARM");
  display.display();
  delay(5000);
  setTime(true, false);
  rtc.adjust(DateTime(setYear, setMonth, setDay, setClockHours, setClockMinutes, 30));
  // GIVE DATA TO CLOCK, READ TEMP AND HYDRATION
  hydr = round(dht.readHumidity());
  temp = round(dht.readTemperature());
  delay(300);
};

void loop() {
  DateTime now = rtc.now();

  // TURN ON AND OFF DISPLAY
  if (digitalRead(buttonPin4) == LOW) {
    if (displayStateOn == true) {
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      displayStateOn = false;
    } else if (displayStateOn == false) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      hydr = round(dht.readHumidity());
      temp = round(dht.readTemperature());
      displayStateOn = true;
    };
    delay(500);
  };
  
  // MOVE TEXT EVERY MINUTE TO DECREASE RISK OF OLED BURN-IN
  currentMinute = now.minute();
  if (currentMinute != lastMinute) {
    if (burnInDisplace == 0) {
      burnInDisplace = 2;
      lastMinute = currentMinute;
    } else if (burnInDisplace == 2) {
      burnInDisplace = 4;
      lastMinute = currentMinute;
    } else if (burnInDisplace == 4) {
      burnInDisplace = 8;
      lastMinute = currentMinute;
    } else if (burnInDisplace == 8) {
      burnInDisplace = 10;
      lastMinute = currentMinute;
    } else {
      burnInDisplace = 0;
    };
  };

  // READ TEMP AND HYDRATION EVERY 30S WHILE DISPLAY IS ON
  if (displayStateOn == true) {
    unsigned long currentMillis1 = millis();
    if (currentMillis1 - lastReadTime1 >= interval30s) {
      lastReadTime1 = currentMillis1;
      hydr = round(dht.readHumidity());
      temp = round(dht.readTemperature());
    };
  };

  // READ AND DISPLAY TIME
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - lastReadTime2 >= interval1s) {
    lastReadTime2 = currentMillis2;
    
    // ADD 0 IN FRONT SINGLE-DIGIT NUMBERS
    if (now.hour() < 10) {
      hour = "0" + String(now.hour(), DEC);
    } else {
      hour = now.hour();
    };
    if (now.minute() < 10) {
      minute = "0" + String(now.minute(), DEC);
    } else {
      minute = now.minute();
    };
    if (now.day() < 10) {
      day = "0" + String(now.day(), DEC);
    } else {
      day = now.day();
    };
    if (now.month() < 10) {
      month = "0" + String(now.month(), DEC);
    } else {
      month = now.month();
    };
    if (temp < 10) {
      tempString = "0" + String(temp);
    } else {
      tempString = temp;
    };
    if (hydr < 10) {
      hydrString = "0" + String(hydr);
    } else {
      hydrString = hydr;
    };

    // DISPLAY TIME AND DATE
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0+burnInDisplace, 0);
    display.println(hour);
    display.setCursor(21+burnInDisplace, 0);
    display.println(":");
    display.setCursor(30+burnInDisplace, 0);
    display.println(minute);
    display.setCursor(60+burnInDisplace, 0);
    display.print(days[now.dayOfTheWeek()]);
    display.setCursor(0+burnInDisplace, 20);
    display.println(day);
    display.setCursor(21+burnInDisplace, 20);
    display.println(".");
    display.setCursor(30+burnInDisplace, 20);
    display.println(month);
    display.setCursor(50+burnInDisplace, 20);
    display.println(".");
    display.setCursor(60+burnInDisplace, 20);
    display.println(now.year(), DEC);
    display.setCursor(0+burnInDisplace, 40);
    display.println(tempString);
    display.setCursor(25+burnInDisplace, 40);
    display.println("C");
    display.setCursor(45+burnInDisplace, 40);
    display.println(hydrString);
    display.setCursor(70+burnInDisplace, 40);
    display.println("%h");
    display.display();
  };
  
  // SET ALARM
  if (digitalRead(buttonPin3) == LOW) {
    setTime(false, true);
    delay(300);
  };

  // TRIGGER ALARM IF CURRENT TIME MATCHES ALARM TIME AND ALARM HAS NOT BEEN STOPPED
  if (setAlarmHours == now.hour() && setAlarmMinutes == now.minute() && alarmStopped == false) {
    if (displayStateOn == false) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      displayStateOn = true;
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("ALARM");
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Press x to stop");
    display.display();

    // SOUND ALARM
    i = 0;
    while (digitalRead(buttonPin3) == HIGH) {
      unsigned long currentMillis3 = millis();
      if (currentMillis3 - lastReadTime3 >= interval2s) {
        lastReadTime3 = currentMillis3;
        if (buzzerState == LOW) {
          buzzerState = HIGH;
        } else {
          buzzerState = LOW;
        };
        digitalWrite(buzzerPin, buzzerState);
        i++;
      };
      if (i >= alarmToneCount) {
        break;
      };
    };

    digitalWrite(buzzerPin, LOW);
    alarmStopped = true;
    hydr = round(dht.readHumidity());
    temp = round(dht.readTemperature());
    delay(300);
  };

  // ENSURE ALARM REPEATS NEXT DAY IF STOPPED
  if (setAlarmHours != now.hour() && setAlarmMinutes != now.minute() && alarmStopped == true) {
    alarmStopped = false;
  };
};

void setTime(bool setClock, bool setAlarmClock) {
  Serial.println(setClock);
  setYear = 2020;
  setMonth = 1;
  setDay = 1;
  setHours = 0;
  setMinutes = 0;
  if (setClock) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Set current");
    display.setCursor(0, 15);
    display.println("date and time");
    display.setCursor(0, 30);
    display.println("Press x to continue");
    display.display();
    while (digitalRead(buttonPin3) == HIGH) {
      delay(20);
    };
  } else if (setAlarmClock) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Set hours and minutes");
    display.setCursor(0, 15);
    display.println("for the alarm.");
    display.setCursor(0, 30);
    display.println("Press x to continue.");
    display.display();
    delay(300);
    alarmStopped = false;
    while (digitalRead(buttonPin3) == HIGH) {
      delay(20);
    };
  };

  if (setClock) {
    // SET CURRENT DATE - SET YEAR
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Year:");
    display.setCursor(0, 20);
    display.println(setYear);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Press x to continue.");
    display.setTextSize(2);
    display.display();
    delay(300);
    while (digitalRead(buttonPin3) == HIGH) {
      // ADJUST THE TIME UP AND DOWN - LIMIT MAX MIN
      if (digitalRead(buttonPin2) == LOW) {
        if (setYear < 9000) {
          setYear = setYear + 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Year:");
          display.setCursor(0, 20);
          display.println(setYear);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
      if (digitalRead(buttonPin1) == LOW) {
        if (setYear > 2000) {
          setYear = setYear - 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Year:");
          display.setCursor(0, 20);
          display.println(setYear);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
    };

    // SET CURRENT DATE - SET MONTH
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Month:");
    display.setCursor(0, 20);
    display.println(setMonth);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Press x to continue.");
    display.setTextSize(2);
    display.display();
    delay(300);
    while (digitalRead(buttonPin3) == HIGH) {
      // ADJUST THE TIME UP AND DOWN - LIMIT MAX MIN
      if (digitalRead(buttonPin2) == LOW) {
        if (setMonth < 12) {
          setMonth = setMonth + 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Month:");
          display.setCursor(0, 20);
          display.println(setMonth);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
      if (digitalRead(buttonPin1) == LOW) {
        if (setMonth > 1) {
          setMonth = setMonth - 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Month:");
          display.setCursor(0, 20);
          display.println(setMonth);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
    };

    // SET CURRENT DATE - SET DAY
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Day:");
    display.setCursor(0, 20);
    display.println(setDay);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Press x to continue.");
    display.setTextSize(2);
    display.display();
    delay(300);
    while (digitalRead(buttonPin3) == HIGH) {
      // ADJUST THE TIME UP AND DOWN - LIMIT MAX MIN
      if (digitalRead(buttonPin2) == LOW) {
        if (setDay < 31) {
          setDay = setDay + 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Day:");
          display.setCursor(0, 20);
          display.println(setDay);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
      if (digitalRead(buttonPin1) == LOW) {
        if (setDay > 1) {
          setDay = setDay - 1;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.println("Day:");
          display.setCursor(0, 20);
          display.println(setDay);
          display.setTextSize(1);
          display.setCursor(0, 40);
          display.println("Press x to continue.");
          display.setTextSize(2);
          display.display();
          delay(170);
        };
      };
    };
  };

  // SET CLOCK TIME OR ALARM TIME - SET HOURS
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Hours:");
  display.setCursor(0, 20);
  display.println(setHours);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("Press x to continue.");
  display.setTextSize(2);
  display.display();
  delay(300);
  while (digitalRead(buttonPin3) == HIGH) {
    if (digitalRead(buttonPin2) == LOW) {
      // ADJUST THE TIME UP AND DOWN - LIMIT MAX MIN
      if (setHours < 24) {
        setHours = setHours + 1;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Hours:");
        display.setCursor(0, 20);
        display.println(setHours);
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.println("Press x to continue.");
        display.setTextSize(2);
        display.display();
        delay(170);
      };
    };
    if (digitalRead(buttonPin1) == LOW) {
      if (setHours > 0) {
        setHours = setHours - 1;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Hours:");
        display.setCursor(0, 20);
        display.println(setHours);
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.println("Press x to continue.");
        display.setTextSize(2);
        display.display();
        delay(170);
      };
    };
  };

  // SET CLOCK TIME OR ALARM TIME - SET MINUTES
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Minutes:");
  display.setCursor(0, 20);
  display.println(setMinutes);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("Press x to continue.");
  display.setTextSize(2);
  display.display();
  delay(300);
  while (digitalRead(buttonPin3) == HIGH) {
    // ADJUST THE TIME UP AND DOWN - LIMIT MAX MIN
    if (digitalRead(buttonPin2) == LOW) {
      if (setMinutes < 59) {
        setMinutes = setMinutes + 1;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Minutes:");
        display.setCursor(0, 20);
        display.println(setMinutes);
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.println("Press x to continue.");
        display.setTextSize(2);
        display.display();
        delay(170);
      };
    };
    if (digitalRead(buttonPin1) == LOW) {
      if (setMinutes > 0) {
        setMinutes = setMinutes - 1;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Minutes:");
        display.setCursor(0, 20);
        display.println(setMinutes);
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.println("Press x to continue.");
        display.setTextSize(2);
        display.display();
        delay(170);
      };
    };
  };

  if (setClock) {
    setClockHours = setHours;
    setClockMinutes = setMinutes;
  } else if (setAlarmClock) {
    setAlarmHours = setHours;
    setAlarmMinutes = setMinutes;
  };
};