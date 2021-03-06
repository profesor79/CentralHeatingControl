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


long boilerStateDebounce = 300000; // do not change boiler state so often
long time3 = -30000;

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

char stateLetter[2] = {'H'};
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

  // put your setup code here, to run once:
  Serial.begin(57600);  // Used to type in characters
  Serial.println("Starting");

  setEEprom();
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  // NOTE: Cursor Position: (CHAR, LINE) start at 0
  lcd.setCursor(0, 0); //Start at character 4 on line 0


  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(200);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running

  pinMode(PUMP,  OUTPUT);
  pinMode(BOILER,  OUTPUT);

  // Start up the library
  blueTempSensor.begin();
  orangeTempSensor.begin();
  greenTempSensor.begin();
  brownTempSensor.begin();


  if (!blueTempSensor.getAddress  ( blueThermometer  , 0)) Serial.println("Un e BLUE");
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
  //  readTemp();
  // handleBoiler();
  //  receiveData();
  radio();
}


void radio()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  //    vw_send((uint8_t *)msg, strlen(msg));
  //    vw_wait_tx(); // Wait until the whole message is gone

  // Wait at most 200ms for a reply
  if (vw_wait_rx_max(1500))
  {
    vw_get_message(buf, &buflen); // Non-blocking

    int i;

    // Message with a good checksum received, dump it.
    vw_rx_stop();
    Serial.print("hot2: ");
    Serial.print(buflen);
    Serial.println(" bytes");
    String eoeo ;
    for (i = 0; i < buflen; i++)
    {
      eoeo += (char)buf[i]   ;
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    
    lcd.setCursor(0, 1);
    lcd.print(eoeo);    
    Serial.println(eoeo);

    Serial.println("Sending data");
    sendData();
    Serial.println("Sending data finshed");
    vw_rx_start();

  }
  else
    Serial.println("Timeout");

}
/*
      for (int i = 0; i < buflen; i++)
      {
        message[i] = (char) buf[i];
      }


      Serial.print("Got123: ");
      Serial.println(String(message));
      Serial.println("Sending data");
      sendData();
      Serial.println("Sending data finshed");

*/

void   handleBoiler() {
  // 0 - stop heating
  // 1 - hot water
  // 2 - standard heating

  //boilerStateDebounce
  if (millis() - time3 > boilerStateDebounce) {
    time3 = millis();



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
  lcd.print("S");
  lcd.print(stateLetter);
  lcd.print(" BP");
  lcd.print(!boilerState);
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
    if ( displayDataPart == 4) {
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
    lcd.print(brownTemperature, 2);
    lcd.print(" gr");
    lcd.print(greenTemperature, 2);

    lcd.print(" ");
  }

  if ( displayDataPart == 3) {
    lcd.print("br");
    lcd.print(brownTemperature, 2);
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


    currentSensor++;

    if (currentSensor == 0) {
      blueTempSensor.requestTemperatures();
      blueTemperature = getTemperature(blueThermometer, blueTempSensor);
    }

    if (currentSensor == 1) {
      orangeTempSensor.requestTemperatures();
      orangeTemperature = getTemperature(orangeThermometer, orangeTempSensor);
    }
    if (currentSensor == 2) {
      greenTempSensor.requestTemperatures();
      greenTemperature = getTemperature(greenThermometer, greenTempSensor);
    }

    if (currentSensor == 3) {
      brownTempSensor.requestTemperatures();
      brownTemperature = getTemperature(brownThermometer, brownTempSensor);
      currentSensor = -1;
    }
    digitalWrite(LED, 0);
  }
}


void sendData()
{


  /*
  frame details

  |--from--|--to--|--messageNo--|--order--|--text--|

  |--from--|--to--|--messageNo--|--order--|--text--|
   123456789012345678901234567890
  "______________________________"
  "00-01-00-00-getAllReadings____"
  "01-00-05-00-inletWater:25.20__"
  "01-00-04-00-outletWater:27.40_"
  "01-00-03-00-outsideTemp:7.40__"
  "01-00-02-00-pump:1____________"
  "01-00-01-00-boiler:1__________"
  "01-00-00-00-t:20161228222600__"

  "00-02-00-00-getAllReadings____"

  */

  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  const int  bufferLen = 20;
  char mesage7[bufferLen];
  String m7 = "m07**-07-pumpState:";
  m7.toCharArray(mesage7, bufferLen - 1);
  const  char *msg7 = mesage7;
  vw_send((uint8_t *)msg7, strlen(mesage7));
  Serial.println(msg7);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg7);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage6[bufferLen];
  //  String m6 = "01-00-06-boilerState:___1234567890123456789";
  String m6 = "m06**56789012345678";
  m6.toCharArray(mesage6,  bufferLen - 1);
  const  char *msg6 = mesage6 ;
  vw_send((uint8_t *)msg6, strlen(mesage6));
  Serial.println(msg6);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg6);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage5[bufferLen];
  //12345678901234567890123456789
  //  String m5 = "01-00-05-outletT:________1234567890123456789";
  String m5 = "m05**56789012345678";
  m5.toCharArray(mesage5,  bufferLen - 1);
  const  char *msg5 = mesage5 ;
  vw_send((uint8_t *)msg5, strlen(mesage5));
  Serial.println(msg5);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg5);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage4[bufferLen];
  //12345678901234567890123456789
  //  String m4 = "01-00-04-inletT:1234________1234567890123456789";
  String m4 = "m04**56789012345678";
  m4.toCharArray(mesage4, bufferLen - 1);
  const  char *msg4 = mesage4 ;
  vw_send((uint8_t *)msg4, strlen(mesage4));
  Serial.println(msg4);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg4);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage3[bufferLen];
  //12345678901234567890123456789
  //  String m3= "01-00-03-outsideT:123456__1234567890123456789";
  String m3 = "m03**56789012345678";
  m3.toCharArray(mesage3, bufferLen - 1);
  const  char *msg3 = mesage3 ;
  vw_send((uint8_t *)msg3, strlen(mesage3));
  Serial.println(msg3);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
    lcd.print(msg3);    

  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage2[bufferLen];
  //  String m2= "01-00-02-insideT:____";
  String m2 = "m02**56789012345678";
  m2.toCharArray(mesage2, bufferLen - 1);
  const  char *msg2 = mesage2 ;
  vw_send((uint8_t *)msg2, strlen(mesage2));
  Serial.println(msg2);
    lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg2);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage1[bufferLen];
  //  String m1 = "01-00-01-state:a*****___";
  String m1 = "m01**56789012345678";
  m1.toCharArray(mesage1, bufferLen - 1);
  const  char *msg1 = mesage1 ;
  vw_send((uint8_t *)msg1, strlen(mesage1));
  Serial.println(msg1);
  lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg1);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage0[bufferLen];
  //  String m0 = "01-00-00-DT:20190103210332";
  String m0 = "m00**3456789012345678";
  m0.toCharArray(mesage0, bufferLen - 1);
  const  char *msg0 = mesage0 ;
  vw_send((uint8_t *)msg0, strlen(mesage0));
  Serial.println(msg0);
  lcd.setCursor(0, 1);
  vw_wait_tx(); // Wait until the whole message is gone
  lcd.print(msg0);    
  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////

}

void receiveData2()
{

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  char message[30];
  if (vw_wait_rx_max(100))
  {

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      char tmp[1];

      for (int i = 0; i < buflen; i++)
      {
        message[i] = (char) buf[i];
      }


      Serial.print("Got123: ");
      Serial.println(String(message));
      Serial.println("Sending data");
      sendData();
      Serial.println("Sending data finshed");
    }
  }

}
