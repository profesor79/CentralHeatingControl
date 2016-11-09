
 
#include <OneWire.h>
//#include <"dth11.h">
#include "dth11.h"


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
#define DHT11PIN 12

#define SENSOR 3 //bedroom 01
//#define SENSOR 4 //bedroom 02

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

centralHeatingData receivedData;

dht11 DHT11;

int humidity;
int temp;

void setup() {
	Serial.begin(9600);
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
		int chk = DHT11.read(DHT11PIN);

		Serial.print("Read sensor: ");
		switch (chk)
		{
		case 0: Serial.println("OK"); break;
		case -1: Serial.println("Checksum error"); break;
		case -2: Serial.println("Time out error"); break;
		default: Serial.println("Unknown error"); break;
		}

		humidity = DHT11.humidity;
		temp = DHT11.temperature;
		Serial.print("Temperature (oC): ");
		Serial.println(temp);

		Serial.print("Humidity (%): ");
		Serial.println(humidity);
	}

	uint8_t rcvdSize = sizeof(receivedData);
	Serial.println("....receive start....");
	if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
	{
		Serial.print("got message to: ");
		Serial.println(receivedData.To);

		//  if (receivedData.To == SENSOR )

		{
			//send readings and current state
			int to = receivedData.From;
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
	Serial.println("....receive stop....");

	delay(2000);

}
