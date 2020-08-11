/*

    Copyright (C) 2020  M Komandur
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

/*  If an object is closer than 25 cms from the Ultrasonic distance sensor
 *  - Waves a small stick on a servo
 *  - Displays "Hello!" on the LCD line 1
 *  If the object is more than 25 cms away from the Ultrasonic distance sensor,
 *  - Displays "The Distance is " on the LCD line 1
 *  In either case, it displays distance in CMs in LCD line 2 followed by " cm(s)."
 */

/*
 * Hardware used
 * - 1602 LCD with I2c
 * - Arduino Mega2560 board
 * - Micro servo motor - 9g, SG90
 * - DHT11 Temperature and Humidity sensor
 * - 10K potentiometer to adjust LCD brightness
 * - Ultra sonic distance sensor HR SC-04
 * - Breadboard
 * - Jumpers
 */

#include "Arduino.h"
#include "Servo.h"
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int trigPin = 10;
const int echoPin = 12;
const int servoPin = 7;
const int mySweepAngle = 10;
const int distanceCMToWaveFlag = 25;
const int dhtPin = 4;
const int dhtWait = 1000;

const float soundSpeedAtZeroC = 331.3;
const float soundSpeedIncreasePerDegreeC = 0.6;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2);

DHT myDHTSensor = DHT(dhtPin,DHT11);

Servo myservo;

// The waveFlag method will move the flag by sweepAngle given in degrees.
void waveFlag(int sweepAngle, bool reverseSweep)
{
	int position = 0;
	int servoWait = 50;

	// This for loop moves the flag in one direction
	for (position = 0; position <= sweepAngle; position += 1)
	{
		myservo.write(position);
		delay(servoWait);
	}
	// This for loop moves the flag in the reverse direction
	if (reverseSweep)
	{
		for (position = sweepAngle; position >= 0; position -= 1)
		{
			myservo.write(position);
			delay(servoWait);
		}
	}

}

//The setup function is called once at startup of the sketch
void setup()
{
	lcd.init();
	lcd.backlight();

	myservo.attach(servoPin);

	myDHTSensor.begin();
	pinMode(dhtPin,INPUT);

	Serial.begin(9600);

	pinMode(trigPin,OUTPUT);
	pinMode(echoPin,INPUT);
}

float getDistanceInCM(){

	digitalWrite(trigPin, LOW); // Silence the pulse
	delayMicroseconds(3); // wait in silence
	digitalWrite(trigPin, HIGH); // send the pulse
	delayMicroseconds(5);
	digitalWrite(trigPin, LOW); // Silence the pulse after 5 microseconds.

	//listen for echo
	float roundTripTimeInMicroSecs = pulseIn(echoPin, HIGH);
	float oneWayTimeInSecs = roundTripTimeInMicroSecs / 1000.0 / 1000.0 / 2.0;
	float distanceInMeters = oneWayTimeInSecs*getSpeedOfSound();
	return distanceInMeters*100; // Returns in centimeters.
}


float getSpeedOfSound(){

	delay(dhtWait);

	return soundSpeedAtZeroC + soundSpeedIncreasePerDegreeC * myDHTSensor.readTemperature();

}


// The loop function is called in an endless loop
void loop()
{


	float distanceNow = getDistanceInCM();

	lcd.setCursor(0,0);
	lcd.print("The Distance is :");
	lcd.setCursor(0,1);
	lcd.print("       ");
	lcd.setCursor(0,1);
	lcd.print(distanceNow);
	lcd.setCursor(6,1);
	lcd.print(" cm(s).");

	Serial.print("The Distance is :");
	Serial.print(distanceNow);
	Serial.println(" centimeters.");




	if (distanceNow < distanceCMToWaveFlag)
	{
		lcd.setCursor(0,0);
		lcd.print("Hello!          ");
		waveFlag(mySweepAngle, true);
		delay(5000);

	}


}
