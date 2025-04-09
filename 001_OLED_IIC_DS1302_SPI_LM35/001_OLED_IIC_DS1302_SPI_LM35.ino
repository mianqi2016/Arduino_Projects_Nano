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

const int lm35_pin = A0;	/* LM35 O/P pin */
unsigned long int lm35Time = 0;
const unsigned int lm35Duration = 30000;   // read LM35 every 30000ms(30s)


void setup() {
  Serial.begin(115200);

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

  int temp_adc_val;
  float temp_val;
  if((millis()-lm35Time)>=lm35Duration) {
    lm35Time=millis();
    temp_adc_val = analogRead(lm35_pin);	/* Read Temperature */
    temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
    temp_val = (temp_val/10);	/* LM35 gives output of 10mv/°C */
    //oled.clear();
  } else {
    ;
  }
  // temp_adc_val = analogRead(lm35_pin);	/* Read Temperature */
  // temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  // temp_val = (temp_val/10);	/* LM35 gives output of 10mv/°C */

  oled.set2X();
  oled.setFont(Callibri15);
  oled.setCursor(20, 2);
  //oled.clear();
  oled.print(temp_val);
  //oled.print("\xC2\xB0"); // shows degree symbol
  //oled.print("\337C"); // shows degree symbol
  //oled.println(" C");
  //oled.println(" °C");
  oled.print(" ");
  oled.setFont(System5x7);
  oled.setCursor(85, 3);
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

