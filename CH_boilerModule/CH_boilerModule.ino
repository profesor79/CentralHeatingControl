// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>


// Data wire is plugged into port 2 on the Arduino
#define BLUE_TEMP_SENSOR 		3
#define ORANGE_TEMP_SENSOR		4
#define GREEN_TEMP_SENSOR 		5
#define BROWN_TEMP_SENSOR 		6

#define PUMP             		7
#define BOILER                          8
#define TEMPERATURE_PRECISION 12

#define SWITCH 9

#define LED 12

#define Water25 14
#define Water35 15
#define Water55 16
#define Water65 17



int switchReading;     // the current reading from the input pin
int state = 0;      // the current state of the system
// 0 start
// 1 heating water 25 deg
// 2 heating water 35 deg
// 3 hot water in cylinder -  water 55 deg
// 4 hot water in cylinder -  water 75 deg
//


// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 400;   // the debounce time, increase if the output flickers
long time2 = 0;
long tempDebounce = 30000;


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
              greenThermometer  ,
              brownThermometer  ;

float    blueTemperature   ,
         orangeTemperature ,
         greenTemperature  ,
         brownTemperature  ;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Read from EEPROM: ");

  int eeAddress = 0; //EEPROM address to start reading from
  int stateEprom =   EEPROM.read(eeAddress);

  Serial.print("Current mode: ");
  Serial.println(stateEprom);

  if (stateEprom == 0 || stateEprom > 4)
  {
    Serial.println("Updating eprom state");
    stateEprom = 3;
    EEPROM.write(eeAddress, stateEprom);

  }

  state = stateEprom;


  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  blueTempSensor.begin();
  orangeTempSensor.begin();
  greenTempSensor.begin();
  brownTempSensor.begin();

  if (!blueTempSensor.getAddress  ( blueThermometer  , 0)) Serial.println("Unable to find address for Device BLUE");
  if (!orangeTempSensor.getAddress( orangeThermometer , 0)) Serial.println("Unable to find address for Device ORANGE");
  if (!greenTempSensor.getAddress ( greenThermometer  , 0)) Serial.println("Unable to find address for Device GREEN");
  if (!brownTempSensor.getAddress ( brownThermometer  , 0)) Serial.println("Unable to find address for Device BROWN");

  // set the resolution to 9 bit per device
  blueTempSensor.setResolution( blueThermometer  , TEMPERATURE_PRECISION);
  orangeTempSensor.setResolution( orangeThermometer, TEMPERATURE_PRECISION);
  greenTempSensor.setResolution( greenThermometer , TEMPERATURE_PRECISION);
  brownTempSensor.setResolution( brownThermometer , TEMPERATURE_PRECISION);

  pinMode(LED, OUTPUT);

  pinMode(Water25,  OUTPUT);
  pinMode(Water35,  OUTPUT);
  pinMode(Water55,  OUTPUT);
  pinMode(Water65,  OUTPUT);

  pinMode(PUMP,  OUTPUT);
  pinMode(BOILER,  OUTPUT);

}


// function to print the temperature for a device
float getTemperature(DeviceAddress deviceAddress, DallasTemperature bus)
{
  return bus.getTempC(deviceAddress);

}


/*
 * Main function, calls the temperatures in a loop.
 */
void loop(void)
{


  handleSwitch();
  readTemp();
}

void readTemp()
{

  if (millis() - time2 > tempDebounce) {
    time2 = millis();
    digitalWrite(LED, 1);
    // call blueTempSensor.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.println("Requesting temperatures...");


    blueTempSensor.requestTemperatures();
    blueTemperature = getTemperature(blueThermometer, blueTempSensor);
    Serial.print("blueTemperature: ");
    Serial.println(blueTemperature);

    orangeTempSensor.requestTemperatures();
    orangeTemperature = getTemperature(orangeThermometer, orangeTempSensor);
    Serial.print("orangeTemperature: ");
    Serial.println(orangeTemperature);


    greenTempSensor.requestTemperatures();
    greenTemperature = getTemperature(greenThermometer, greenTempSensor);
    Serial.print("greenTemperature: ");
    Serial.println(greenTemperature);

    brownTempSensor.requestTemperatures();
    brownTemperature = getTemperature(brownThermometer, brownTempSensor);
    Serial.print("brownTemperature: ");
    Serial.println(brownTemperature);

    Serial.println("DONE");


    digitalWrite(LED, 0);


  }
}


void handleSwitch()
{

  switchReading = digitalRead(SWITCH);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (millis() - time > debounce) {
    time = millis();
    state = state + switchReading;
    if (state > 4) {
      state = 1;
    }




    if (state == 1)
    {
      //#define Water25 14
      Serial.println("W25");
      if (blueTemperature > 25 + 1) {
        digitalWrite( BOILER, 0);
      } else {
        digitalWrite( PUMP, 1);
        digitalWrite( BOILER, 1);
      }

    }

    if (state == 2)
    {

      Serial.println("W35");
      //#define Water35 14
      //55 deg
      if (blueTemperature > 35 + 1) {
        digitalWrite( BOILER, 0);
      } else {
        digitalWrite( PUMP, 1);
        digitalWrite( BOILER, 1);
      }

    }

    if (state == 3)
    {
      //55 deg
      Serial.println("W55");
      if (blueTemperature > 55 + 1) {
        digitalWrite( BOILER, 0);
        digitalWrite(LED, 1);
        //wait 25 sec to coll down    delay(15000);
        delay(25000);
        digitalWrite( PUMP, 0);
        digitalWrite(LED, 0);
      } else {
        digitalWrite( PUMP, 1);
        digitalWrite( BOILER, 1);
      }

    }

    if (state == 4)
    {
      Serial.println("W65");
      //65 deg
      if (blueTemperature > 65 + 1) {
        digitalWrite( BOILER, 0);
        //wait 25 sec to coll down    delay(15000);
        digitalWrite(LED, 1);
        delay(25000);
        digitalWrite( PUMP, 0);
        digitalWrite(LED, 0);
      } else {
        digitalWrite( PUMP, 1);
        digitalWrite( BOILER, 1);
      }

    }

    Serial.print("PUMP");
    Serial.println(digitalRead(PUMP));

    Serial.print("BOILER: ");
    Serial.println(digitalRead(BOILER));

    digitalWrite(Water25, state == 1);
    digitalWrite(Water35, state == 2);
    digitalWrite(Water55, state == 3);
    digitalWrite(Water65, state == 4);


  }
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
