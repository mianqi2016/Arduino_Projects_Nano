/*
  LiquidCrystal Library - Blink

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD and makes the
 cursor block blink.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
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
DHT11 dht11(10);

int temperature = 0;
int humidity = 0;

int result = 0; // the temperature and humidity values from the DHT11 sensor

boolean flagDHT11 = false; // flag for DHT11 read error

const int durationDHT11 = 30000; // 30 seconds internal for DHT11 stable working
unsigned long readDHT11 = 0;  // last time read DHT11

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

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

  //get the date and time the compiler was run
  //next 25 lines should be commented for 3v battery keep time accuratelly
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }

  // Attempt to read the temperature and humidity values from the DHT11 sensor.
  result = dht11.readTemperatureHumidity(temperature, humidity);
}

void loop() {
    if (RTC.read(tm)) {
      if(flagDS1307 == false) {
        ;
      } else {
        //Serial.print("Ok, Time = ");
        lcd.setCursor(6, 0);
        print2digits(tm.Hour);
        lcd.print(':');
        print2digits(tm.Minute);
        lcd.print(':');
        print2digits(tm.Second);
        //Serial.print(", Date (D/M/Y) = ");
      }

      if(tm.Day>=10 && tm.Month>=10) {
        lcd.setCursor(6, 3);
      } else if(tm.Day<10 || tm.Month<10) {
        lcd.setCursor(7, 3);
      } else {    // tm.Day<10 and tm.Month>=10 or otherwise
        lcd.setCursor(7, 3);
      }

      lcd.print("Day:");
      lcd.print(tm.Day);
      lcd.print('/');
      lcd.print(tm.Month);
      lcd.print('/');
      lcd.print(tmYearToCalendar(tm.Year)); 
      //Serial.println();
    } else {
        if (RTC.chipPresent()) {
          Serial.println("The DS1307 is stopped.  Please run the SetTime");
          Serial.println("example to initialize the time and begin running.");
          Serial.println();
        } else {
          Serial.println("DS1307 read error!  Please check the circuitry.");
          Serial.println();
        }
      }
    //delay(1000);
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
      if(flagDHT11 == true) {
        ;
      } else {
        lcd.clear();  // clear only once any reset
        flagDHT11 = true;
      }
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
        lcd.setCursor(6, 0);
        print2digits(tm.Hour);
        lcd.print(':');
        print2digits(tm.Minute);
        lcd.print(':');
        print2digits(tm.Second);
        flagDS1307 = false;   // don't show h/m/s for no overlap on screen
    }

  lcd.noBlink();
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print('0');
  }
  lcd.print(number);
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
