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

#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSoftSpi.h"

SSD1306AsciiSoftSpi oled;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

unsigned long int oledTime = 0;
const unsigned int oledDuration = 1000;   // read LM35 every 1000ms(1 time per second)

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

/* DS18B20 1-Wire digital temperature sensor with Arduino example code. More info: https://www.makerguides.com */

// Include the required Arduino libraries:
#include "OneWire.h"
//#include "DallasTemperature.h"

OneWire  ds(8);  // on pin 8 (a 4.7K resistor is necessary)

// Create a new instance of the oneWire class to communicate with any OneWire device:
//OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to DallasTemperature library:
//DallasTemperature sensors(&oneWire);

unsigned long int timeDS18B20 = 0;
const unsigned int durationDS18B20 = 30000;   // read LM35 every 30000ms(30s)

byte i;
byte present = 0;
byte type_s;
byte data[9];
byte addr[8];

//float celsius, fahrenheit;
// Fetch the temperature in degrees Celsius for device index:
//float tempC = sensors.getTempCByIndex(0); // the index 0 refers to the first device
float tempC = 0;
// Fetch the temperature in degrees Fahrenheit for device index:
//float tempF = sensors.getTempFByIndex(0);


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

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();
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
        oled.setCursor(22, 3);     // Start at top-left corner
        oled.print(tempC);
        oled.write(' ');
        oled.write("\200C");
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
  if((millis()-timeDS18B20)>=durationDS18B20) {
    timeDS18B20=millis();
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    //celsius = (float)raw / 16.0;
    //fahrenheit = celsius * 1.8 + 32.0;
    tempC = (float)raw / 16.0;    
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

