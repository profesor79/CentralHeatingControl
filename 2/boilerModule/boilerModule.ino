#include <DallasTemperature.h>
/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // Include the libraries we need
#include <OneWire.h>
#include <EEPROM.h>
#include <VirtualWire.h>




#define DS3231_I2C_ADDRESS  0x68
#define BLUE_TEMP_SENSOR 		7
#define ORANGE_TEMP_SENSOR		8
#define GREEN_TEMP_SENSOR 		5
#define BROWN_TEMP_SENSOR 		6
#define TEMPERATURE_PRECISION           12
#define TX_MODULE                       10
#define RX_MODULE                       11
#define PUMP             		14
#define BOILER                          15
#define LED                      13
#define On 0   // inversed logic for relays
#define Off 1
#define hotWaterBase              64
#define onOffBase                196
#define hotWaterTempertaureBase   16
#define boilerTempertaureBase     17

#define   HotWaterTemp "HotWaterTemperature"
#define ShuttingBoiler "ShuttingBoiler"

struct centralHeatingData
{
  uint8_t    From = 0;
  uint8_t    To = 255;
  uint8_t    Command = 0;
  uint8_t    Data0 = 0;
  uint8_t    Data1 = 0;
};

//                       00  01     02  03     04  05     06  07     08  09     10  11     12  13     14  15     16  17     18  19     20  21     22  23
byte    HotWater[84] = {B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000,
                        B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00111110, B00110011, B11110000
                       };


//                    00  01     02  03     04  05     06  07     08  09     10  11     12  13     14  15     16  17     18  19     20  21     22  23
byte    OnOff[84] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
                     B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
                    };




long tempDisplayDebounce = 5000;
long timeTempDisplay = -30000;  // force temp reading
short displayDataPart = 0; // what will be displayed on the bottom line
bool displayDataPartFlag = false;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
centralHeatingData receivedData;

long time = 0;         // the last time the output pin was toggled
long debounce = 400;   // the debounce time, increase if the output flickers
long time2 = -30000;  // force temp reading
long tempDebounce = 500;
short currentSensor = 0;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire blueTemp(BLUE_TEMP_SENSOR);
OneWire orangeTemp(ORANGE_TEMP_SENSOR	);
OneWire  greenTemp(GREEN_TEMP_SENSOR 	);
OneWire  brownTemp(BROWN_TEMP_SENSOR 	);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature blueTempSensor(&blueTemp);
DallasTemperature orangeTempSensor(&orangeTemp);
DallasTemperature  greenTempSensor(&greenTemp);
DallasTemperature  brownTempSensor(&brownTemp);




// arrays to hold device addresses
DeviceAddress blueThermometer   ,
              orangeThermometer ,
              brownThermometer,
              greenThermometer  ;

float    blueTemperature   = 70,
         orangeTemperature = 70,
         brownTemperature = 70,
         greenTemperature  = 70;


int boilerState = Off, pumpState = Off;
int requestedTemp = 27;
int switchReading;     // the current reading from the input pin
int state = 2;      // the current state of the system
// 0 stop
// 1 heating water
// 2 standard heating


// function to print the temperature for a device
float getTemperature(DeviceAddress deviceAddress, DallasTemperature bus)
{
  return bus.getTempC(deviceAddress);
}

void setEEprom() {

  int eeAddress = 0; //EEPROM address to start reading from
  int stateEprom =   EEPROM.read(eeAddress);

  Serial.println(stateEprom);

  if (stateEprom != 128)
  {

    for (int i = 0; i < 84; i++) {
      EEPROM.write(hotWaterBase + i, HotWater[i]);
    }

    for (int i = 0; i < 84; i++) {
      EEPROM.write(onOffBase + i, OnOff[i]);
    }

    EEPROM.write(hotWaterTempertaureBase, 65);
    EEPROM.write(boilerTempertaureBase,   30);
    EEPROM.write(eeAddress,              128);
  }
}

void setup() {
  setEEprom();
  // put your setup code here, to run once:
  Serial.begin(9600);  // Used to type in characters
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  // NOTE: Cursor Position: (CHAR, LINE) start at 0
  lcd.setCursor(0, 0); //Start at character 4 on line 0


  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(500);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running

  pinMode(PUMP,  OUTPUT);
  pinMode(BOILER,  OUTPUT);

  // Start up the library
  blueTempSensor.begin();
  orangeTempSensor.begin();
  greenTempSensor.begin();
  brownTempSensor.begin();


  if (!blueTempSensor.getAddress  ( blueThermometer  , 0)) Serial.println("Unable BLUE");
  if (!orangeTempSensor.getAddress( orangeThermometer , 0)) Serial.println("Unable ORANGE");
  if (!greenTempSensor.getAddress ( greenThermometer  , 0)) Serial.println("Unable GREEN");
  if (!brownTempSensor.getAddress ( brownThermometer  , 0)) Serial.println("Unable brown");


  // set the resolution to 12 bit per device
  blueTempSensor.setResolution( blueThermometer  , TEMPERATURE_PRECISION);
  orangeTempSensor.setResolution( orangeThermometer, TEMPERATURE_PRECISION);
  greenTempSensor.setResolution( greenThermometer , TEMPERATURE_PRECISION);
  brownTempSensor.setResolution( brownThermometer , TEMPERATURE_PRECISION);

}

void loop() {
  // put your main code here, to run repeatedly:
  displayTime();
  readTemp();
  handleBoiler();
  delay(100);
}


void   handleBoiler() {
  // 0 - stop heating
  // 1 - hot water
  // 2 - standard heating

  if (state == 0)
  {
    Serial.println("Boiler stop");
    boilerState = Off;
    pumpState = Off;
  }

  if (state == 1)
  {
    Serial.print(HotWaterTemp);
    Serial.println( requestedTemp);
    if (blueTemperature > requestedTemp + 1) {
      boilerState = Off;
      pumpState = Off;
      digitalWrite(LED, 1);
      Serial.print(ShuttingBoiler);

      digitalWrite( BOILER, boilerState);
      //wait 25 sec to coll down
      delay(25000);
    }

    if (blueTemperature < requestedTemp + 1) {
      boilerState = On;
      pumpState = On;
    }
  }

  if (state > 1 )
  {

    Serial.print("standard heating: temp:");
    Serial.println( requestedTemp);
    if (blueTemperature > requestedTemp + 1) {
      boilerState = Off;
      pumpState = On;
    }

    if (blueTemperature < requestedTemp - 1) {
      boilerState = On;
      pumpState = On;
    }

  }

  digitalWrite( BOILER, boilerState);
  digitalWrite( PUMP, pumpState);


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
if (hour < 10)
  {
    lcd.print("0");
  }
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

  lcd.print("B/P ");
  lcd.print(!boilerState);
  lcd.print("/");
  lcd.print(!pumpState);




  lcd.setCursor(0, 1);
  // display 4 d


  if ( second % 5 == 0) {    
    if ( displayDataPartFlag ) {
      displayDataPart++;
    }
    else
    {
      displayDataPartFlag = false;
    }
    if ( displayDataPart == 5) {
      displayDataPart = 0;
    }
  }
  else
  {
    displayDataPartFlag = true;
  }

  if ( displayDataPart == 0) { // display date
    lcd.print(dayOfMonth, DEC);
    lcd.print("/");
    lcd.print(month, DEC);
    lcd.print("/");
    lcd.print(year, DEC);
    lcd.print("             ");
  }


  if ( displayDataPart == 1) { // display blue and orange sense
    lcd.print("bl");
    lcd.print(blueTemperature, 2);
    lcd.print(" re");

    lcd.print(requestedTemp, DEC);
    lcd.print(" ");
  }


  if ( displayDataPart == 2) { // display blue and orange sense
    lcd.print("br");
    lcd.print(brownTemperature,2);
    lcd.print(" gr");
        lcd.print(greenTemperature,2);

    lcd.print(" ");
  }

  if ( displayDataPart == 3) {
    lcd.print("br");
    lcd.print(brownTemperature,2);
    lcd.print(" or");
    lcd.print(orangeTemperature, 2);


    lcd.print(" ");
  }



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


void readTemp()
{

  if (millis() - time2 > tempDebounce) {
    time2 = millis();
    digitalWrite(LED, 1);
    // call blueTempSensor.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.println("Requesting temperatures...");

    currentSensor++;

    if (currentSensor == 0) {
      blueTempSensor.requestTemperatures();
      blueTemperature = getTemperature(blueThermometer, blueTempSensor);
      Serial.print("blueTemp: ");
      Serial.println(blueTemperature);
      Serial.println(currentSensor);
    }

    if (currentSensor == 1) {
      orangeTempSensor.requestTemperatures();
      orangeTemperature = getTemperature(orangeThermometer, orangeTempSensor);
      Serial.print("orangeTem: ");
      Serial.println(orangeTemperature);
      Serial.println(currentSensor);
    }
    if (currentSensor == 2) {
      greenTempSensor.requestTemperatures();
      greenTemperature = getTemperature(greenThermometer, greenTempSensor);
      Serial.print("greenTemp: ");
      Serial.println(greenTemperature);
      Serial.println(currentSensor);
    }

    if (currentSensor == 3) {
      brownTempSensor.requestTemperatures();
      brownTemperature = getTemperature(brownThermometer, brownTempSensor);
      Serial.print("brownTemp: ");
      Serial.println(brownTemperature);
      Serial.println(currentSensor);
      currentSensor = -1;
    }

    Serial.println("DONE");
    digitalWrite(LED, 0);
  }
}

