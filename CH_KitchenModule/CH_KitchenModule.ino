// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <VirtualWire.h>

// Data wire is plugged into port 2 on the Arduino
#define BLUE_TEMP_SENSOR 		3
#define ORANGE_TEMP_SENSOR		4
#define GREEN_TEMP_SENSOR 		5
#define BROWN_TEMP_SENSOR 		6

#define BEDROOMS             		7
#define LIVINGROOMS                     8
#define TEMPERATURE_PRECISION 12

#define SWITCH 9

#define TX_MODULE 10
#define RX_MODULE 11


struct centralHeatingData
{
  int    From;
  int    Seq;
  int    To = -1;
  int    Command ;
  int    Ack;
  float    Data1 = 0;
  float    Data2 = 0;
  float    Data3 = 0;
  float    Data4 = 0;
  float    Data5 = 0;
  float    Data6 = 0;
};

centralHeatingData HData;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(2000);	 // Bits per sec
  //    vw_rx_start();       // Start the receiver PLL running
}

void loop() {
  // put your main code here, to run repeatedly:
  // HData receivedData2 ; //  = {2,1,1,1,0,1,2,3,4,5,6,"0123456789"};
  HData.From = 2;
  HData.Seq = 1;
  HData.To = 1;
  HData.Command = 2 ;
  HData.Ack = 1;
  HData.Data1 = 0;
  HData.Data2 = 0;
  HData.Data3 = 0;
  HData.Data4 = 0;
  HData.Data5 = 0;
  HData.Data6 = 0;
  //HData.strData='012345678';
  int c = 1;
  for (int i = 1; i < 40; i++) {
    if (c > 3) {
      c = 0;
    }
    
    HData.Command = c++ ;
    HData.Data1 = i;

    Serial.println("START: ");

    vw_send((uint8_t *)&HData, sizeof(HData));

    //    vw_send((uint8_t *)&receivedData2, sizeof( receivedData2));
    vw_wait_tx();

    Serial.println("STOP: ");
    digitalWrite(13, 0);
    digitalWrite(13, 1);
    digitalWrite(13, 0);
    digitalWrite(13, 1     );
    delay(100);
    digitalWrite(13, 0);
    delay(5000);
  }
}
