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
- 4x4 Keypad to enter Alarm time
- Buzzer for alarm
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
#include <Keypad.h> // keypad

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <RTC.h>
static DS3231 rtc;

// Define DHTPin:
#define DHTPin 2 // Digital Humidity and Temperature sensor is on this pin

// Define SDA and SCL pin from LCD:
#define SDAPin 20 // Data pin
#define SCLPin 21 // Clock pin

// Define Buzzer Pin
#define BuzzerPin 3 // Buzzer pin


// Define DHT sensor type:
#define DHTType DHT11


const char* daysOfTheWeek[] =
 {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; //days of the week
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


// The below are for the keypad


#define lengthOfTimeText 5 // alarm time is entered as "hh*mm". So it is 5 characters long 


byte keyPressCount= 0;

const byte ROWS = 4;
const byte COLS = 4;
const int ALARM_NO = 1;
const int CLOCK_REFRESH_MILL_SECS = 1050; // DHT sensor takes slightly longer than a second to get readings. Else it would give NaN (Not A Number) readings. Adjust this for your DHT sensor
const int BUZZER_KHZ = 500; //Frequency (Kilo Hertz) of buzzer's audio frequency
const int NO_OF_BEEPS_SET = 2; // number of beeps for each alarm

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {4, 5, 6, 7}; 
byte colPins[COLS] = {8, 9, 11, 12}; // I skipped pin 10 as my jumpers were crowded 


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char keyPressed = customKeypad.getKey();


int secondsForAlarmTimeEntered = 0;

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

  // Set buzzer - pin is BuzzerPin, as an output
  pinMode(BuzzerPin, OUTPUT); 


 
}

void loop() {
   //showClockInSerialMonitor();
   int chk = dht.read(DHTPin);
   writeTimeOnLCD(1,0,true); // display time from clock on first line and also show seconds
   writeDateOnLCD(1,7,true); // display date from clock on first line and also show month in words
   writeDayOfWeekOnLCD(2,0); // display time and date from clock on first line
   writeTemperatureOnLCD(2,5, true); //display temperature on second line and show as Fahrenheit
   writeHumidityOnLCD(2,12); //display humidity on second line

   // keep checking if keypad is pressed
   keyPressed = customKeypad.getKey();

   if (keyPressed == 'A'){
      enterAlarmTimeInKeypad(); 
   }

   if (secondsForAlarmTimeEntered > 0) { // means alarm is set
      int waitingToBeep = secondsForAlarmTimeEntered - getSecondsSinceMidnightForNow();
      if ((waitingToBeep <= 1) && (waitingToBeep >= -1)) {
        beepBuzzer(BUZZER_KHZ); // beep the buzzer. Display refresh delay is taken care when beeping
        Serial.println("Alarm Triggered - Beeping");
      } else {
        Serial.print("Alarm is Set - Waiting to beep in :- ");Serial.println(waitingToBeep);
        delay(CLOCK_REFRESH_MILL_SECS); 
      }
      

   } else {
      //Serial.println("Refreshing without Beeps");

      delay(CLOCK_REFRESH_MILL_SECS); 
 
   }


}

void beepBuzzer(int freqInKhz) {
  for (int beepCount = 0; beepCount < NO_OF_BEEPS_SET; beepCount++)
  {
  
    tone(BuzzerPin, freqInKhz);
    Serial.print("Beeping on BuzzerPin : count = " + beepCount);
    Serial.println(BuzzerPin);
    delay(CLOCK_REFRESH_MILL_SECS); // beep at the same intervals as CLOCK_REFRESH_MILL_SECS
    noTone(BuzzerPin);
  }
  secondsForAlarmTimeEntered = 0; // unset the alarm

}



void enterAlarmTimeInKeypad(){
  Serial.println("Inside enterAlarmTimeInKeypad() call");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Beep at? 24h*mm");
  lcd.setCursor(6,1);
  int tempNum = 0, hr = 0, mn = 0;
  keyPressCount = 0;
  while (keyPressCount < lengthOfTimeText) {
      keyPressed = customKeypad.getKey();
      if (keyPressed){
        if (keyPressed != '*') { 
            if (hr == 0) {
              tempNum = (keyPressed - 48) + tempNum*10; // subtract 48 because ascii for 0 is 48
              Serial.print("Key Pressed :");
              Serial.println(keyPressed);
              Serial.print("Temp Hour :");
              Serial.println(tempNum);
            } else {
              mn = (keyPressed - 48) + mn*10; // subtract 48 because ascii for 0 is 48
              Serial.print("Minutes :");
              Serial.println(mn);
            }
            lcd.print(keyPressed); 
        } else {
            hr = tempNum;
            lcd.print(":"); // when * key is pressed on the keyPad, show it as : in the LCD 
        }
          
        keyPressCount++; 
        
    }
  } 
  setAlarmOnLocal(hr,mn);
  
  
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.println("Alarm Set For :");
  lcd.setCursor(5,1);
  writeAsTwoDigits(hr);
  lcd.print(":");
  writeAsTwoDigits(mn);
  delay(5000);
  lcd.clear();
  
}

int getSecondsSinceMidnightForNow() {
  return rtc.getHours() * 3600 + rtc.getMinutes()*60 + rtc.getSeconds();

}  

void setAlarmOnLocal(int hr, int mn) {
  Serial.println("Inside setAlarmOnRTC() call");
  secondsForAlarmTimeEntered = hr * 3600 + mn * 60;

  
}

void writeAsTwoDigits(int num) { //this adds a 0 before single digit numbers
  if (num >= 0 && num < 10) {
    lcd.write('0');
  }
  lcd.print(num);
}


void findAndSetRTC() { // finds and sets real time clock

  rtc.begin();

  Serial.print("Is Clock Running: ");
  if (rtc.isRunning())
  {
    Serial.println("Yes");
    Serial.print(rtc.getDay());
    Serial.print("-");
    Serial.print(rtc.getMonth());
    Serial.print("-");
    Serial.print(rtc.getYear());
    Serial.print(" ");
    Serial.print(rtc.getHours());
    Serial.print(":");
    Serial.print(rtc.getMinutes());
    Serial.print(":");
    Serial.print(rtc.getSeconds());
    Serial.print("");
    if (rtc.getHourMode() == CLOCK_H12)
    {
      switch (rtc.getMeridiem()) {
      case HOUR_AM:
        Serial.print(" AM");
        break;
      case HOUR_PM:
        Serial.print(" PM");
        break;
      }
    }
    Serial.println("");
    delay(1000);
  }
  else
  {
    delay(1500);

    Serial.println("No");
    Serial.println("Setting Time");
    //rtc.setHourMode(CLOCK_H12);
    rtc.setHourMode(CLOCK_H24);
    rtc.setDateTime(__DATE__, __TIME__);
    Serial.println("New Time Set");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
    rtc.startClock();
  }
  rtc.disableAlarm1();
  rtc.enableAlarm1();

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
  writeAsTwoDigits(rtc.getHours());
  lcd.print(":");
  writeAsTwoDigits(rtc.getMinutes());
  if (showSeconds) {
    lcd.print(":");
    writeAsTwoDigits(rtc.getSeconds());
  }
}

void writeDateOnLCD(int lcdLineNo, int cursorPos, boolean monthInWords) { //  usually cursorPos 7
  lcdLineNo--;
  if (monthInWords) {
    cursorPos += 2;
  }
  lcd.setCursor(cursorPos,lcdLineNo); 
  writeAsTwoDigits(rtc.getDay());
  if (monthInWords) {
    lcd.print(monthsOfTheYear[rtc.getMonth()]);
  } else {
    lcd.print("/");
    writeAsTwoDigits(rtc.getMonth());
    lcd.print("/");
  }
  lcd.print(rtc.getYear()-2000);
}


void writeDayOfWeekOnLCD(int lcdLineNo, int cursorPos) { // usually cursorPos 14
  lcdLineNo--;
  lcd.setCursor(cursorPos,lcdLineNo); 
  lcd.print(daysOfTheWeek[rtc.getWeek()-1]); 
  lcd.print("  ");
}
