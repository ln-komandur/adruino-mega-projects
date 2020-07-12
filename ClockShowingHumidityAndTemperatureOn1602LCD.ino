/*  Clock shows Time (HH:MM;SS), date (DDMMMYY) on LCD line 1, Day of week (DDD), Temperature (F), Humidity (%) on LCD line 2
    Copyright (C) 2020  L Komandur

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


/*
Hardware used
- 1602 LCD with  i2c
- Arduino Mega2560 board
- RTC is DS3231
- DHT11 Temperature and Humidity sensor
- 10K potentiometer to adjust LCD brightness 
- Breadboard
- Jumpers

By editing respective parameters to method calls in this program, the display can be made to show
- time without seconds
- month as numbers
- temperature in celcius

*/

#include <Adafruit_Sensor.h> // Include Adafruit sensor library https://github.com/adafruit/Adafruit_Sensor
#include <DHT.h> // Include Adafruit DHT library https://github.com/adafruit/DHT-sensor-library
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <RTClib.h>
RTC_DS3231 rtc;

// Define DHTPin:
#define DHTPin 4 // Digital Humidity and Temperature sensor is on this pin

// Define SDA and SCL pin from LCD:
#define SDAPin 20 // Data pin
#define SCLPin 21 // Clock pin

// Define DHT sensor type:
#define DHTType DHT11


const char* daysOfTheWeek[] =
 { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; //days of the week
const char* monthsOfTheYear[] =
 {"Dec", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov" }; //days of the week

byte thermometer[8] = //icon for thermometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte droplet[8] = //icon for water droplet 
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

// Create a DHT sensor object:
DHT dht = DHT(DHTPin,DHTType);



// Connect to LCD via i2c, default address 0x27 (A0-A2 not jumpered):
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2);

// Create an object to capture current timestamp
DateTime now;



void setup() {
  // Initialize dht sensor
  dht.begin();

  // read the DHTPin and know the status
  int chkDHTSensor = dht.read(DHTPin);

  // Initialize LCD:
  lcd.init();
  // Can toggle between back light and noBacklight by uncommenting only one of the lines below
  //lcd.noBacklight(); lcd.display();
  lcd.backlight(); /* or /* lcd.setBacklight(1);*/

  // create custom characters on the LCD for thermometer and water droplet
  lcd.createChar(1,thermometer);
  lcd.createChar(2,droplet);

  // Begin Serial communication at a baudrate of 9600:
  Serial.begin(9600);
  // find and set the RTC
  findAndSetRTC();

 
}

void loop() {
   //showClockInSerialMonitor();
   now = rtc.now();
   int chk = dht.read(DHTPin);
   writeTimeOnLCD(1,0,true); // display time from clock on first line and also show seconds
   writeDateOnLCD(1,7,true); // display date from clock on first line and also show month in words
   writeDayOfWeekOnLCD(2,0); // display time and date from clock on first line
   writeTemperatureOnLCD(2,5, true); //display temperature on second line and show as Fahrenheit
   writeHumidityOnLCD(2,12); //display humidity on second line
   delay(1050); // DHT sensor takes slightly longer than a second to get readings. Else it would give NaN (Not A Number) readings. Adjust this for your DHT sensor

}

void writeAsTwoDigits(int num) { //this adds a 0 before single digit numbers
  if (num >= 0 && num < 10) {
    lcd.write('0');
  }
  lcd.print(num);
}


void findAndSetRTC() { // finds and sets real time clock

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  } else {
    Serial.println("RTC power is in order. Time unsure!");
  }

}


void findRTCAndSetCustom(int yyyy,int mon, int dd, int hh, int mm, int amOrPm) { // finds real time clock and sets to custom date
    // This line sets the RTC with an explicit date & time, 
    // for example to set to "January 21, 2014 at 3:00am", call with parameters as below
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    rtc.adjust(DateTime(yyyy,mon, dd, hh, mm, amOrPm));

}


void writeTemperatureOnLCD(int lcdLineNo, int cursorPos, boolean isFahrenheit) { //  usually cursorPos is 5
  lcdLineNo--;

  lcd.setCursor(cursorPos, lcdLineNo); 
  lcd.write(1); // display the thermometer sign
  lcd.print("      "); // Clear the display before writing. In case NaN has appeared
  lcd.setCursor(cursorPos+1, lcdLineNo);

  lcd.print((float)dht.readTemperature(isFahrenheit),0); // true means Fahrenheit. void is Celsius
  lcd.print((char)223); //degree sign
  if (isFahrenheit) {
      lcd.print("F"); // Fahrenheit 
  } else {
      lcd.print("C"); // Celcius
  }

}

// returns the reading as a float
void writeHumidityOnLCD(int lcdLineNo, int cursorPos) { //  usually cursorPos 12
  lcdLineNo--;  
  lcd.setCursor(cursorPos, lcdLineNo);
  lcd.write(2); // display the water drop sign
  lcd.print("    "); // Clear the display before writing. In case NaN has appeared
  lcd.setCursor(cursorPos+1, lcdLineNo);
  lcd.print((float)dht.readHumidity(), 0);
  lcd.print("%");
  
}


void writeTimeOnLCD(int lcdLineNo, int cursorPos, boolean showSeconds) { //  usually cursorPos 0

  lcdLineNo--;
  lcd.setCursor(cursorPos, lcdLineNo);
  writeAsTwoDigits(now.hour());
  lcd.print(":");
  writeAsTwoDigits(now.minute());
  if (showSeconds) {
    lcd.print(":");
    writeAsTwoDigits(now.second());
  }
}

void writeDateOnLCD(int lcdLineNo, int cursorPos, boolean monthInWords) { //  usually cursorPos 7
  lcdLineNo--;
  if (monthInWords) {
    cursorPos += 2;
  }
  lcd.setCursor(cursorPos,lcdLineNo); 
  writeAsTwoDigits(now.day());
  if (monthInWords) {
    lcd.print(monthsOfTheYear[now.month()]);
  } else {
    lcd.print("/");
    writeAsTwoDigits(now.month());
    lcd.print("/");
  }
  lcd.print(now.year()-2000);
}


void writeDayOfWeekOnLCD(int lcdLineNo, int cursorPos) { // usually cursorPos 14
  lcdLineNo--;
  lcd.setCursor(cursorPos,lcdLineNo); 
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]); 
}


void showClockInSerialMonitor() {

    now = rtc.now();
    Serial.print("RTC Time :- ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(':');
/*
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
*/
    Serial.print(" RTC Temperature:- ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

}
