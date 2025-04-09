/*
  LiquidCrystal Library - Blink

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD and makes the
 cursor block blink.

 The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalBlink

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/**
 * DHT11 Sensor Reader
 * This sketch reads temperature and humidity data from the DHT11 sensor and prints the values to the serial port.
 * It also handles potential error states that might occur during reading.
 *
 * Author: Dhruba Saha
 * Version: 2.1.0
 * License: MIT
 */

// Include the DHT11 library for interfacing with the sensor.
#include <DHT11.h>
// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
// - For ESP32: Connect the sensor to pin GPIO2 or P2.
// - For ESP8266: Connect the sensor to GPIO2 or D4.
DHT11 dht11(12);

int temperature = 0;
int humidity = 0;

int result = 0; // the temperature and humidity values from the DHT11 sensor

boolean flagDHT11 = false; // flag for DHT11 read error

const int durationDHT11 = 30000; // 30 seconds internal for DHT11 stable working
unsigned long readDHT11 = 0;  // last time read DHT11

// DS1302
// CONNECTIONS:
// DS1302 CLK/SCLK --> D15/A1
// DS1302 DAT/IO --> D14/A0
// DS1302 RST/CE --> D16/A2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <RtcDS1302.h>

ThreeWire myWire(14,15,16); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// SetTime on DS1307
bool parse=false;
bool config=false;

boolean flagDS1307 = false; // flag for DS1307 h/m/s showing when DHT11 read error

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  Serial.begin(9600);
  // Print a message to the LCD.
  //lcd.print("hello, world!");

  // Attempt to read the temperature and humidity values from the DHT11 sensor.
  result = dht11.readTemperatureHumidity(temperature, humidity);

  // set time on DS1302
  //  Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop() {
    // display time info on LCD2004A
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    //delay(10000); // ten seconds

    if(millis() - readDHT11 >= durationDHT11) {
        // Attempt to read the temperature and humidity values from the DHT11 sensor.
        result = dht11.readTemperatureHumidity(temperature, humidity);
        readDHT11 = millis();
    } else {
      ;
    }
    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    //int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
      // if(flagDHT11 == true) {
      //   ;
      // } else {
      //   lcd.clear();  // clear only once any reset
      //   flagDHT11 = true;
      // }
        lcd.setCursor(0, 1);
        lcd.print("Temperature: ");
        lcd.print(temperature);
        lcd.print("\337C");
        //lcd.print(" °C");
        lcd.setCursor(0, 2);
        lcd.print("Humidity: ");
        lcd.print(humidity);
        lcd.print("%");
        lcd.setCursor(0, 3);
        lcd.blink();
        flagDS1307 = true;
    } else {
        // Print error message based on the error code.
        //lcd.println(DHT11::getErrorString(result));
         lcd.setCursor(0, 1);
         lcd.print("DHT11 startup...");
    }

  lcd.noBlink();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    //Serial.print(datestring);
    //lcd.print(datestring);

    lcd.setCursor(6, 0);
    print2digits(dt.Hour());
    lcd.print(':');
    print2digits(dt.Minute());
    lcd.print(':');
    print2digits(dt.Second());

    if(dt.Day()>=10 && dt.Month()>=10) {
        lcd.setCursor(6, 3);
      } else if(dt.Day()<10 || dt.Month()<10) {
        lcd.setCursor(7, 3);
      } else {    // tm.Day<10 and tm.Month>=10 or otherwise
        lcd.setCursor(7, 3);
      }

    lcd.print("Mon:");
    lcd.print(dt.Month());
    lcd.print('.');
    lcd.print(dt.Day());
    lcd.print('.');
    lcd.print(dt.Year());
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print('0');
  }
  lcd.print(number);
}
