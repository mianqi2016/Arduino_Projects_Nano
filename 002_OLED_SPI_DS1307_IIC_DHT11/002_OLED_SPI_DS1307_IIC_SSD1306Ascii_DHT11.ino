// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
// OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//Comment out above, uncomment this block to use hardware SPI
// #define OLED_DC     8
// #define OLED_CS     10
// #define OLED_RESET  9
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//   &SPI, OLED_DC, OLED_RESET, OLED_CS);

unsigned long int oledTime = 0;
const unsigned int oledDuration = 1000;   // read LM35 every 1000ms(1 time per second)

#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSoftSpi.h"

SSD1306AsciiSoftSpi oled;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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
DHT11 dht11(8);

unsigned long int timeDHT11 = 0;
const unsigned int durationDHT11 = 30000;   // read LM35 every 30000ms(30s)

int temperature = 0;
int humidity = 0;


void setup() {
  Serial.begin(9600);

  // Use next line if no RST_PIN or reset is not required.
  // oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, CLK_PIN, MOSI_PIN);  
  oled.begin(&Adafruit128x64,OLED_CS, OLED_DC, OLED_CLK, OLED_MOSI, OLED_RESET);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!");
  delay(1000);
  oled.clear();

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
}

void loop() {
  if (RTC.read(tm)) {
    if(flagDS1307 == false) {
      ;
    } else {
      //if(millis()-oledTime>=oledDuration) {
        //oled.clear();
        //oledTime=millis();
        // Return to standard size.
        oled.set1X();
        oled.setCursor(32, 0);     // Start at top-left corner

        display2digits(tm.Hour);
        oled.write(':');
        display2digits(tm.Minute);
        oled.write(':');
        display2digits(tm.Second);

        oled.set2X();
        oled.setFont(System5x7);
        //oled.setCursor(85, 3);
        oled.setCursor(5, 2);
        oled.print("Temp:");
        //oled.print("Temperature: ");
        //oled.set2X();         
        oled.print(temperature);
        oled.print("\200C");
        
        oled.setCursor(5, 4);
        oled.print("Hum:");
        //oled.set1X();
        //oled.print("Humidity: ");
        oled.set2X();
        oled.print(humidity);
        oled.print("%");

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

    if(tm.Day>=10 && tm.Month>=10) {
      oled.setCursor(0, 7);
    } else if(tm.Day<10 || tm.Month<10) {
      oled.setCursor(10, 7);
    } else {    // tm.Day<10 and tm.Month>=10 or otherwise
      oled.setCursor(5, 7);
    }
        
    // Return to standard size.
    oled.set1X();
    //oled.write("Month-");
    //oled.write("Date(D/M/Y)= ");
    oled.write("(M/D/Y)=");
    display2digits(tm.Month);
    oled.write('.');
    display2digits(tm.Day);
    oled.write(',');
    display2digits(tm.Year+1970);

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

  //int temp_adc_val;
  //float temp_val;
  if((millis()-timeDHT11)>=durationDHT11) {
    timeDHT11=millis();
        // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
    timeDHT11 = millis();    // Attempt to read the temperature and humidity values from the DHT11 sensor.

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
    //oled.clear();
    //oled.set2X();
    //oled.setCursor(22, 3);     // Start at top-left corner
    //oled.print(temp_val);
    //oled.write(' ');
    //oled.write("\200C");

    
  } else {
    ;
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
    oled.write('0');
  }
  oled.print(number);
}

