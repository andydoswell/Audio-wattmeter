/* The "I'm sick of that analogue meter" Audio wattmeter
   
(c) A.G.Doswell 2nd April 2016
 License: The MIT License (See full license at the bottom of this file)

  V1.0 

  Arduino measures the DC output from a diode bridge across the load resistor,
  does the maths and displays the results. Peak-hold feature, averaging and PWM fan control too!

  This is only going to give true readings with a sine wave input.

  VRMS = VPk + 1.41  /  1.414
  Power = VRMS^2 / R

  R is either 4 or 8 ohms (switchable)

*/

boolean rPin;
int sensorValue;
float voltagePk;
float voltageRMS;
float powerR;
float powerL;
float loadR;
float peakR = 0.0;
float peakL = 0.0;
int loopCounter = 0;
float averageL = 0;
float averageR = 0;
int averageCounter = 0;
float averagePowerL = 0;
float averagePowerR = 0;
int fanPWM = 0;
int fanPWM2 = 0;

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialise the LCD

void setup() {

  pinMode (7, INPUT_PULLUP); // switch connected to 7 defines load resistance, Open for ~8 ohms, short to GND for ~4 Ohms.
  lcd.begin (16, 2);
}

void loop() {
  rPin = digitalRead (7);
  if (rPin == false) {
    loadR = 4.7; // My 4 ohm load is actually 4.7 ohms
  }
  else {
    loadR = 8.0; // .. but my 8 ohm load is spot on!
  }

  sensorValue = analogRead(A0); // Output from Right channel connected to A0
  voltagePk = sensorValue * (35.0 / 1023.0); //Convert the analogue reading to a voltage
  voltageRMS = (voltagePk) / 1.414; // Vpk to VRMS
  powerR = (sq(voltageRMS)) / loadR; // VRMS and load to power
  if (powerR > peakR) { // update peak value
    peakR = powerR;
  }
  averageR = averageR + powerR; //running average

  sensorValue = analogRead(A1); // Output from Left channel connected to A0
  voltagePk = sensorValue * (35.0 / 1023.0); //Convert the analogue reading to a voltage
  voltageRMS = (voltagePk) / 1.414; // Vpk to VRMS
  powerL = (sq(voltageRMS)) / loadR; // VRMS and load to power
  if (powerL > peakL) { // update peak value
    peakL = powerL;
  }
  averageL = averageL + powerL; //running average

fanPWM2 =  (powerR + powerL) * 8; // get a value for the fan speed PWM output
  if (fanPWM2 > fanPWM) { // if it's greater than the current PWM value, then up date it
      fanPWM = fanPWM2;
    }

   if (fanPWM > 255) { // limit the maximum PWM value to 255 (flat out)
    fanPWM=255;
   }
   
  analogWrite (6, fanPWM); // write it to the 

  if (averageCounter == 100) { // sets the Average reset
    
    averagePowerL = averageL / averageCounter; // create the two averages from the running value
    averagePowerR = averageR / averageCounter;
    averageCounter = 0; // reset the counter
    averageL = 0; // reset the running averages
    averageR = 0;
    fanPWM --; // decrement the PWM value for the fan if this are cooling down!
  }

  if (loopCounter == 1000) { // when this has run 1000 times reset the peak value.
    loopCounter = 0;
    peakL = 0;
    peakR = 0;

  }
  lcd.setCursor (0, 0);
  lcd.print (powerL, 1);
  lcd.print ("W ");
  lcd.print (averagePowerL, 1);
  lcd.print ("W ");
  lcd.print (peakL, 1);
  lcd.print ("W ");
  lcd.setCursor (0, 1);
  lcd.print (powerR, 1);
  lcd.print ("W ");
  lcd.print (averagePowerR, 1);
  lcd.print ("W ");
  lcd.print (peakR, 1);
  lcd.print ("W ");
  lcd.print (loadR);

  loopCounter++; // decrement the counters
  averageCounter ++;
}

/*
   Copyright (c) 2016 Andrew Doswell

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permiSerialion notice shall be included in
   all copies or substantial portions of the Software.

   Any commercial use is prohibited without prior arrangement.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESerial FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
