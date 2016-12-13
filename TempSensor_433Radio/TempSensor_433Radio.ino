                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // Include the libraries we need
#include <OneWire.h>
#include "dth11.h"
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <VirtualWire.h>
#include <Wire.h>  // Comes with Arduino IDE


// Data wire is plugged into port 2 on the Arduino
#define BLUE_TEMP_SENSOR 		3
#define ORANGE_TEMP_SENSOR		4
#define GREEN_TEMP_SENSOR 		5
#define BROWN_TEMP_SENSOR 		6

#define BEDROOMS             		77
#define LIVINGROOMS                     8
#define TEMPERATURE_PRECISION 12
#define SWITCH 9

#define TX_MODULE 10
#define RX_MODULE 11
#define DHT11PIN 12

#define SENSOR 3 //bedroom 01
//#define SENSOR 4 //bedroom 02

#define DS3231_I2C_ADDRESS 0x68


struct centralHeatingData
{
  uint8_t    From = 0;
  uint8_t    To = 255;
  uint8_t    Command = 0;
  uint8_t    Data0 = 0;
  uint8_t    Data1 = 0;
};

long tempDebounce = 30000;
long time2 = -30000;  // force temp reading

long tempDisplayDebounce = 5000;
long timeTempDisplay = -30000;  // force temp reading


centralHeatingData receivedData;

dht11 DHT11;

int humidity;
int temp;

// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address




void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  // put your setup code here, to run once:
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(500);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
}


void loop() {


  if (millis() - time2 > tempDebounce) {

    time2 = millis();
    timeTempDisplay = millis();
    int chk = DHT11.read(DHT11PIN);
    lcd.clear();
    lcd.print("Read sensor: ");
    switch (chk)
    {
      case 0: lcd.print("OK    "); break;
      case -1: lcd.print("Checksum error"); break;
      case -2: lcd.print("Time out error"); break;
      default: lcd.print("Unknown error"); break;
    }

    humidity = DHT11.humidity;
    temp = DHT11.temperature;
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(temp);
    lcd.print("C ");
    lcd.print("H: ");
    lcd.print(humidity);
    lcd.print("%     ");


  }
  else {
    if ((millis() - timeTempDisplay > tempDisplayDebounce)) {
      displayTime(); // display the real-time clock data on the Serial Monitor,
    }

  }

  uint8_t rcvdSize = sizeof(receivedData);
  Serial.println( "....receive start....");
  if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
  {
    lcd.print("got message to: ");
    Serial.println(    receivedData.To);

    //  if (receivedData.To == SENSOR )

    {
      //send readings and current state
      int to =  receivedData.From;
      receivedData.To = to;
      receivedData.From = SENSOR;
      receivedData.Data0 = humidity;
      receivedData.Data1 = temp;

      Serial.print("START: ");
      Serial.println(sizeof(receivedData));
      vw_send((uint8_t *)&receivedData, sizeof(receivedData));
      vw_wait_tx();
      Serial.println("STOP: ");
    }
  }
  Serial.println( "....receive stop....");

  delay(150);

}
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  lcd.setCursor(0, 0);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                 &year);
  // send it to the serial monitor
  lcd.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  lcd.print(":");
  if (minute < 10)
  {
    lcd.print("0");
  }
  lcd.print(minute, DEC);
  lcd.print(":");
  if (second < 10)
  {
    lcd.print("0");
  }
  lcd.print(second, DEC);
  lcd.print(" ");
  
     lcd.print(temp);
    lcd.print("C/");
    lcd.print(humidity);
        lcd.print("%");
  lcd.setCursor(0, 1);    

  lcd.print(dayOfMonth, DEC);
  lcd.print("/");
  lcd.print(month, DEC);
  lcd.print("/");
  lcd.print(2000 + year, DEC);
  lcd.print(" ");
  switch (dayOfWeek) {
    case 1:
      lcd.print("Sun");
      break;
    case 2:
      lcd.print("Mon");
      break;
    case 3:
      lcd.print("Tue");
      break;
    case 4:
      lcd.print("Wed");
      break;
    case 5:
      lcd.print("Thu");
      break;
    case 6:
      lcd.print("Fri");
      break;
    case 7:
      lcd.print("Sat");
      break;
  }
       lcd.print("   ");

  
     lcd.print(temp);
    lcd.print("/");
    lcd.print(humidity);
  
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}
