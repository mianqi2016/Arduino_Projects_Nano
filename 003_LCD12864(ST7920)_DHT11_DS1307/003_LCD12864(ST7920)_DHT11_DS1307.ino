/*

  HelloWorld.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
//  LCD12864        Arduino UNO
//  RS              10
//  R/W             11
//  E               13
//  RST             8

// End of constructor list

//frequency for calling "u8g2.clearBuffer();"
unsigned long int lcd12864Time = 0;
const unsigned int lcd12864Duration = 10;   // call read "u8g2.clearBuffer();" every 10ms(100 times per second)

//DS1307 using "DS1307RTC" by Michael Margolis 
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

bool flagDS1307 = true;


void setup(void) {
  u8g2.begin();
  Serial.begin(9600);

  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  // Serial.begin(9600);
  // while (!Serial) ; // wait for Arduino Serial Monitor
  // delay(200);
  // if (parse && config) {
  //   Serial.print("DS1307 configured Time=");
  //   Serial.print(__TIME__);
  //   Serial.print(", Date=");
  //   Serial.println(__DATE__);
  // } else if (parse) {
  //   Serial.println("DS1307 Communication Error :-{");
  //   Serial.println("Please check your circuitry");
  // } else {
  //   Serial.print("Could not parse info from the compiler, Time=\"");
  //   Serial.print(__TIME__);
  //   Serial.print("\", Date=\"");
  //   Serial.print(__DATE__);
  //   Serial.println("\"");
  // }
}

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
#include <DHT11.h>  //Author: Dhruba Saha

// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
// - For ESP32: Connect the sensor to pin GPIO2 or P2.
// - For ESP8266: Connect the sensor to GPIO2 or D4.
//DHT11 dht11(2);
DHT11 dht11(7);

unsigned long int timeDHT11 = 0;
const unsigned int durationDHT11 = 30000;   // read LM35 every 30000ms(30s)

int temperature = 0;
int humidity = 0;

void loop(void) {
  // u8g2.clearBuffer();					// clear the internal memory
  // u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  // u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  // u8g2.sendBuffer();					// transfer internal memory to the display
  // delay(1000);
  if(millis()-lcd12864Time>=lcd12864Duration) {
    u8g2.clearBuffer();
    lcd12864Time = millis();
  } else {
    ;
  }

  if((millis()-timeDHT11)>=durationDHT11) {

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    // if (result == 0) {
    //     Serial.print("Temperature: ");
    //     Serial.print(temperature);
    //     Serial.print(" °C\tHumidity: ");
    //     Serial.print(humidity);
    //     Serial.println(" %");
    // } else {
    //     // Print error message based on the error code.
    //     Serial.println(DHT11::getErrorString(result));
    // }
    timeDHT11 = millis();    // Attempt to read the temperature and humidity values from the DHT11 sensor.

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    // if (result == 0) {
    //     Serial.print("Temperature: ");
    //     Serial.print(temperature);
    //     Serial.print(" °C\tHumidity: ");
    //     Serial.print(humidity);
    //     Serial.println(" %");
    // } else {
    //     // Print error message based on the error code.
    //     Serial.println(DHT11::getErrorString(result));
    // }
  }

  if (RTC.read(tm)) {
    if(flagDS1307 == false) {
      ;
    } else {
      //if(millis()-oledTime>=oledDuration) {
        //oled.clear();
        //oledTime=millis();
        // Return to standard size.
        //u8g2.clearBuffer();					// clear the internal memory
        //u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
        //u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
        u8g2.setCursor(41, 10);
        //u8g2.setFont(u8g2_font_10x20_tf);
        //u8g2.setFont();
        u8g2.setFont(u8g2_font_profont11_mr);
        display2digits(tm.Hour);
        //u8g2.write(':');
        u8g2.print(':');
        display2digits(tm.Minute);
        //u8g2.write(':');
        u8g2.print(':');
        display2digits(tm.Second);

        u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
        //u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
        
        u8g2.setCursor(5, 27);
        u8g2.print("Temperature: ");
        u8g2.setFont(u8g2_font_timB14_tf);
        u8g2.print(temperature);
        u8g2.setFont(u8g2_font_9x15_t_symbols);
        //u8g2.print("176");  //"°"
        u8g2.drawUTF8(99, 27, "°");
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.setCursor(107, 27);
        u8g2.print("C");
        u8g2.setCursor(5, 45);
        u8g2.print("Himidity: ");
        u8g2.setFont(u8g2_font_timB14_tf);
        u8g2.print(humidity);
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.setCursor(85, 45);
        u8g2.print('%');

        // oled.set2X();
        // oled.setCursor(22, 3);     // Start at top-left corner
        // oled.print(temp_val);
        // oled.write(' ');
        // oled.write("\200C");

        //u8g2.sendBuffer();					// transfer internal memory to the display
        //delay(1000);
        //delay(1);


    if(tm.Day>=10 && tm.Month>=10) {
      u8g2.setCursor(15, 60);
    } else if(tm.Day<10 || tm.Month<10) {
      u8g2.setCursor(35, 60);
    } else {    // tm.Day<10 and tm.Month>=10 or otherwise
      u8g2.setCursor(25, 60);
    }
    
    //u8g2.clearBuffer();					// clear the internal memory
    // Return to standard size.
    //oled.set1X();
    //u8g2.write("Month-");
    u8g2.setFont(u8g2_font_simple1_tf);
    //u8g2.setFont(u8g2_font_simple1_tf);
    u8g2.print("Month - ");
    display2digits(tm.Month);
    //u8g2.write('.');
    u8g2.print('.');
    display2digits(tm.Day);
    //u8g2.write(',');
    u8g2.print(',');
    display2digits(tm.Year+1970);
    u8g2.sendBuffer();					// transfer internal memory to the display
    //delay(1000);
      //} else {
        ;
          // Return to standard size.
          // oled.set1X();
          // oled.setCursor(32, 0);     // Start at top-left corner

          // display2digits(tm.Hour);
          // oled.write(':');
          // display2digits(tm.Minute);
          // oled.write(':');
          // display2digits(tm.Second);
      }
    //}
  } else {
      // if (RTC.chipPresent()) {
      //   Serial.println("The DS1307 is stopped.  Please run the SetTime");
      //   Serial.println("example to initialize the time and begin running.");
      //   Serial.println();
      // } else {
      //   Serial.println("DS1307 read error!  Please check the circuitry.");
      //   Serial.println();
      // }
    }
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

void display2digits(int number) {
  if (number >= 0 && number < 10) {
    //u8g2.write('0');
    u8g2.print('0');
  }
  u8g2.print(number);
}
