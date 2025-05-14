// Sharp IR GP2Y0A21YK0F Sensor Example Code with Distance in Inches

const int irPin = A0;  // Analog pin connected to sensor OUT
float voltage, distanceCM, distanceInches;

unsigned long timeBeep = 0;
unsigned int durationBeep = 300; // 300 ms

#define DISTANCE_ALERT 20 // cm

#define LED_PIN A3
#define BEEP_PIN A2

/*
 *The LiquidCrystal library works with all LCD displays that are compatible with the
 *Hitachi HD44780 driver. There are many of them out there, and you
 *can usually tell them by the 16-pin interface.
 *  The circuit:
 * LCD RS pin to digital pin 9
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

/*
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

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);




void setup() {
  Serial.begin(9600); // Initialize Serial Monitor
  pinMode(LED_PIN, OUTPUT);
  pinMode(BEEP_PIN, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
}

void loop() {

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
  // print "ms" - millisecond
  lcd.print(" ms");


  voltage = analogRead(irPin) * (5.0 / 1023.0); // Convert analog value to voltage
  distanceCM = 27.86 * pow(voltage, -1.15);     // Calculate distance in cm
  //distanceInches = distanceCM * 0.393701;       // Convert cm to inches
  if(distanceCM <= DISTANCE_ALERT) {
    durationBeep = distanceCM * 10;
    alert(durationBeep);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BEEP_PIN, LOW);;
  }

  // set the cursor to column 0, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print the distance of target to SHARP 2Y0A21:
  lcd.print("CM:IR = ");
  // set the cursor to column 0, line 0
  lcd.setCursor(9, 0);
  // print the distance of target to SHARP 2Y0A21:
  lcd.print(distanceCM);
  delay(10);

  // Serial.print("Voltage: ");
  // Serial.print(voltage);
  // Serial.print(" V, Distance: ");
  // Serial.print(distanceCM);
  // Serial.print(" cm, ");
  // Serial.print(distanceInches);
  // Serial.println(" inches");

  //delay(500); // Wait 500ms before next reading
}

void alert(unsigned int duration) {
  if(millis()-timeBeep>=duration) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    digitalWrite(BEEP_PIN, !digitalRead(BEEP_PIN));
    timeBeep = millis();
  } else {
    ;
  }
}
