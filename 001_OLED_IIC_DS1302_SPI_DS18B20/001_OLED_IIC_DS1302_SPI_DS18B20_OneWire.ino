// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.
//
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;

// DS1302
// CONNECTIONS:
// DS1302 CLK/SCLK --> D10
// DS1302 DAT/IO --> D11
// DS1302 RST/CE --> D12
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <RtcDS1302.h>

ThreeWire myWire(11,10,12); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// SetTime on DS1307
bool parse=false;
bool config=false;

boolean flagDS1302 = false; // flag for DS1302 h/m/s showing when DHT11 read error

/* DS18B20 1-Wire digital temperature sensor with Arduino example code. More info: https://www.makerguides.com */

// Include the required Arduino libraries:
#include "OneWire.h"
//#include "DallasTemperature.h"

OneWire  ds(9);  // on pin 9 (a 4.7K resistor is necessary)

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
  Serial.begin(115200);

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

  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif // RST_PIN >= 0
    // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(Adafruit5x7);

  // set time on DS1302

  // Serial.print("compiled: ");
  // Serial.print(__DATE__);
  // Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //printDateTimeOLED(compiled);
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
    
/*
  uint32_t m = micros();
  oled.clear();
  oled.println("Hello world!");
  oled.println("A long line may be truncated");
  oled.println();
  oled.set2X();
  oled.println("2X demo");
  oled.set1X();
  oled.print("\nmicros: ");
  oled.print(micros() - m);
  */

}

void loop() {
// display time info on LCD2004A
  RtcDateTime now = Rtc.GetDateTime();

  printDateTimeOLED(now);

  // if((millis()-timeDS18B20)>=durationDS18B20) {
  //   // Send the command for all devices on the bus to perform a temperature conversion:
  //   sensors.requestTemperatures();
  //   //tempC = 0;
  //   // Fetch the temperature in degrees Celsius for device index:
  //   tempC = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  //   timeDS18B20 = millis();
  // } else {
  //   ;
  // }
  // temp_adc_val = analogRead(lm35_pin);	/* Read Temperature */
  // temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  // temp_val = (temp_val/10);	/* LM35 gives output of 10mv/°C */

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.

  if((millis()-timeDS18B20)>=durationDS18B20) {
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
  }


  oled.set2X();
  oled.setFont(Callibri15);
  oled.setCursor(20, 2);
  //oled.clear();
  //oled.print(tempC);

  // Print the temperature in Celsius in the Serial Monitor:
  // Serial.print("Temperature: ");
  // Serial.print(tempC);
  // Serial.print(" \xC2\xB0"); // shows degree symbol
  // Serial.println("C");
  //Serial.print("C  |  ");

  //oled.print("\xC2\xB0"); // shows degree symbol
  //oled.print("\337C"); // shows degree symbol
  //oled.println(" C");
  //oled.println(" °C");
  oled.print(" ");
  oled.setFont(System5x7);
  //oled.setCursor(85, 3);
  oled.setCursor(25, 3);
  oled.print(tempC);
  oled.print("\200C");
  //oled.set3X();
  //oled.print("C");
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTimeOLED(const RtcDateTime& dt)
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

    oled.setCursor(40, 0);
    oled.setFont(Verdana12_bold);
    oled.set1X();
    print2digitsOLED(dt.Hour());
    oled.print(':');
    print2digitsOLED(dt.Minute());
    oled.print(':');
    print2digitsOLED(dt.Second());

    
    if(dt.Day()>=10 && dt.Month()>=10) {
        oled.setCursor(50, 6);
      } else if(dt.Day()<10 || dt.Month()<10) {
        oled.setCursor(50, 6);
      } else {    // tm.Day<10 and tm.Month>=10 or otherwise
        oled.setCursor(50, 6);
      }

    oled.set1X();
    oled.setFont(Arial14);

    oled.print("Mon:");
    oled.print(dt.Month());
    oled.print('.');
    oled.print(dt.Day());
    oled.print('.');
    oled.print(dt.Year());

 }

 void print2digitsOLED(int number) {
   if (number >= 0 && number < 10) {
     oled.print('0');
   }
   oled.print(number);
 }

