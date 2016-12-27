#include <VirtualWire.h>

#define TX_MODULE 10
#define RX_MODULE 11

#define MODULEID 1


struct centralHeatingData
{
	int    From = 0;
	int    To = 255;
	int    Command = 0;
	int    Data0 = 0;
	int    Data1 = 0;
};


centralHeatingData receivedData;
centralHeatingData sendingData;


void setup()
{

	pinMode(13, OUTPUT);

	Serial.begin(9600);	// Debugging only
	Serial.println("setup");
	pinMode(13, OUTPUT);

	// Initialise the IO and ISR
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_setup(2000);	 // Bits per sec
	vw_set_tx_pin(TX_MODULE);
	vw_set_rx_pin(RX_MODULE);

	vw_rx_start();       // Start the receiver PLL running

	Serial.println("done");

}

void loop()
{

	for (int i = 2; i < 11; i++)
	{
		blink(i, 50); // msg receiwed
		sendingData.To = i; // just to scan network of devices
		sendingData.Command = i;
		sendingData.From = MODULEID;
		sendingData.Data0 = 1;
		sendingData.Data1 = 98;

		Serial.print("Round: ");
		Serial.print(i);
		Serial.print("  START: ");
		Serial.println(sizeof(sendingData));

		vw_send((uint8_t *)&sendingData, sizeof(sendingData));
		vw_wait_tx();

		Serial.println("STOP: ");

		uint8_t rcvdSize = sizeof(receivedData);
		//wait 2 seconds
		if (vw_wait_rx_max(2000))
		{

			blink(3, 30); // msg receiwed


			if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
			{
				
				Serial.print("receivedData.From 	");
				Serial.println(receivedData.From);
				Serial.print("receivedData.To 	");
				Serial.println(receivedData.To);
				Serial.print("receivedData.Command	");
				Serial.println(receivedData.Command);
				Serial.print("receivedData.Data0	");
				Serial.println(receivedData.Data0);
				Serial.print("receivedData.Data1	");
				Serial.println(receivedData.Data1);
				
				blink(5, 50); // msg receiwed
			}

			
		}
		else
		{
			Serial.println("no data received...");
		}
		
	}

}

void blink(int times, int del) {
	for (int a = 0; a < times; a++) {
		digitalWrite(13, 1);
		delay(del);
		digitalWrite(13, 0);
		delay(del);
	}
}
