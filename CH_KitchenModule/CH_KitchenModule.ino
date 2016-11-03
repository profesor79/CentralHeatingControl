#include <definitions.h>

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

struct centralHeatingData receivedData;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(1000);	 // Bits per sec
  //    vw_rx_start();       // Start the receiver PLL running
}

void loop() {
  // put your main code here, to run repeatedly:
 centralHeatingData receivedData2 ;
 receivedData2.To=1;
 receivedData2.Command=1;
 receivedData2.Data1=40;
 
 

  Serial.println("START: ");
  digitalWrite(13, 0);
  digitalWrite(13, 1);
  digitalWrite(13, 0);
  digitalWrite(13, 1);
  
  vw_send((uint8_t *)&receivedData2, sizeof( receivedData2));
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
