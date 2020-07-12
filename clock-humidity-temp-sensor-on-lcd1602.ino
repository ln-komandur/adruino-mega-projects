/*
Clock shows 
- Time (HH:MM;SS), date (DDMMMYY) on LCD line 1
- Day of week (DDD), Temperature (F), Humidity (%) on LCD line 2

This uses
- 1602 LCD with  i2c
- Arduino Mega board
- RTC is DS3231
- DHT11 Temperature and Humidity sensor
- 10K potentiometer to adjust LCD brightness 
- Breadboard
- Jumpers


This code has code snippets for ultra sonic distance sensor that are currently not invoked (called) - yet  to clean up

*/

#include <Adafruit_Sensor.h> // Include Adafruit sensor library https://github.com/adafruit/Adafruit_Sensor
#include <DHT.h> // Include Adafruit DHT library https://github.com/adafruit/DHT-sensor-library
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <RTClib.h>
RTC_DS3231 rtc;

// Define Trig pin, Echo pin and DHTPin:
#define trigPin 9
#define echoPin 10
#define DHTPin 4 // Digital Humidity and Temperature

// Define SDA and SCL pin from LCD:
#define SDAPin 20 // Data pin
#define SCLPin 21 // Clock pin

// Define DHT sensor type:
#define DHTType DHT11


const char* daysOfTheWeek[] =
 { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"}; //days of the week
const char* monthsOfTheYear[] =
 {"Dec", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov" }; 

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
DateTime now;


void setup() {
  // Define inputs and outputs:
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  dht.begin();

  // Initiate the LCD:
  lcd.init();
  // Toggle between back light and no back light by uncommenting only one of the lines below
  //lcd.noBacklight(); lcd.display();
  lcd.backlight(); /* or /* lcd.setBacklight(1);*/

  lcd.createChar(1,thermometer);
  lcd.createChar(2,droplet);
  


  // Begin Serial communication at a baudrate of 9600:
  Serial.begin(9600);
  findAndSetRTC();

 
}

void loop() {
   //showClockInSerialMonitor();
   now = rtc.now();
   writeTimeOnLCD(1,0,true); // display time from clock on first line
   writeDateOnLCD(1,7,true); // display date from clock on first line
   writeDayOfWeekOnLCD(2,0); // display time and date from clock on first line
   writeTemperatureOnLCD(2,5, true); //display humidity and temperature on second line
   writeHumidityOnLCD(2,12); //display humidity and temperature on second line
   delay(1050); // DHT sensor takes slightly longer than a second to read numbers. Else gives NaN (Not A Number) readings

  // writeDistanceAndSpeedOnLCD(0); //displaying distance and speed only when using Ultra Sound Sensor
   
}

void writeAsTwoDigits(int num) { //this adds a 0 before single digit numbers
  if (num >= 0 && num < 10) {
    lcd.write('0');
  }
  lcd.print(num);
}


void findAndSetRTC() {

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

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));


}


void writeTemperatureOnLCD(int lcdLineNo, int cursorPos, boolean isFahrenheit) { // cursorPos is 5
  lcdLineNo--;
  int chk = dht.read(DHTPin);
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
void writeHumidityOnLCD(int lcdLineNo, int cursorPos) { // cursorPos 12
  lcdLineNo--;  
  lcd.setCursor(cursorPos, lcdLineNo);
  lcd.write(2); // display the water drop sign
  lcd.print("    "); // Clear the display before writing. In case NaN has appeared
  lcd.setCursor(cursorPos+1, lcdLineNo);

  lcd.print((float)dht.readHumidity(), 0);
  lcd.print("%");
  
}



void writeTimeOnLCD(int lcdLineNo, int cursorPos, boolean showSeconds) { // cursorPos 0

  lcdLineNo--;
  //lcd.setCursor(1, lcdLineNo);
  lcd.setCursor(cursorPos, lcdLineNo);
  writeAsTwoDigits(now.hour());
  lcd.print(":");
  writeAsTwoDigits(now.minute());
  if (showSeconds) {
    lcd.print(":");
    writeAsTwoDigits(now.second());
  }
}
void writeDateOnLCD(int lcdLineNo, int cursorPos, boolean monthInWords) { // cursorPos 7
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

void writeDayOfWeekOnLCD(int lcdLineNo, int cursorPos) {
  lcdLineNo--;
    // The 2 lines below are used for displaying day of the week, cursorPos 14
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


// the below is only when using ultra sound sensor

void writeDistanceAndSpeedOnLCD(int lcdLineNo)
{
  // Define variables:
  long duration;
  int distance;
  float speedofsound;
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin. This returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);

  // Read the temperature:
  int temperature = dht.readTemperature();

  // Calculate speed of sound in m/s:
  speedofsound = 331.3+(0.606*temperature);

  // Calculate the distance in cm:
  distance = duration*(speedofsound/10000)/2;

  // Print the distance and temperature on the Serial Monitor:
  lcd.setCursor(0,0);
  lcd.print("Temperature: ");
  lcd.print(temperature);
  lcd.print(" " "\xDF" "C ");
  //lcd.setCursor(0,1);
  lcd.print("Speed: ");
  lcd.print(speedofsound);
  lcd.print(" m/s ");
  lcd.scrollDisplayLeft();
  delay(200);
  lcd.setCursor(0,1);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm  ");
  delay(200);

 
}
