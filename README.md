This alarm clock displays the time, temperature and air humidity. It was programmed in the Arduino IDE. Here is a link to a showcase of the clock: https://www.youtube.com/watch?v=vGX_7jlnlnA

Components: 
Display: 0.96 inch OLED IIC GME12864-41  
Real-time clock: DS3231 RTC  
Microcontroller: ESP32 DOIT Devkit V1  
Temperature and air hydration sensor: KY-015 DHT11  
Buzzer: Active buzzer  

Libaries used:  
Adafruit BusIO v1.16.2 by Adafruit  
Adafruit GFX Library v1.11.11 by Adafruit  
Adafruit SSD1306 v2.5.13 by Adafruit  
Adafruit Unified Sensor v1.1.14 by Adafruit  
DHT sensor library v1.4.6 by Adafruit  
RTClib v2.1.4 by Adafruit  
Wire library by Arduino  
Math library by Arduino  
SPI library by Arduino  

Features:  
The clock is extremely accurate.  
The display can be turned on and off with a button, this also turns on and off the temperature and air hydration sensor to save energy.  
The text moves a bit every minute to decrease the risk of OLED burn-in.  
The alarm turns the display on.  
The alarm rings 20 times before turning off in case the user is not present, the alarm can be stopped before by pushing a button.  
The alarm doesn't have to be set again for the following days.  
The alarm time can be changed as many times as one wants by pushing a button.   
The temperature and humidity sensor checks the temperature and humidity every 30 seconds to save energy.  
The displayed time is updated every second as opposed to several times a second to save energy.  

Circuit diagram: 
![Alarm Clock Circuit Diagram](https://github.com/williamfroeyen/esp32-alarm-clock/blob/master/alarm-clock-circuit-diagram.jpg)