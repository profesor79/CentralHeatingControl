
#include <VirtualWire.h>

#define TX_MODULE 10
#define RX_MODULE 11

#define MODULEID 4


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

	uint8_t rcvdSize = sizeof(receivedData);
	if (vw_wait_rx_max(2000)) {
		blink(3, 80); // msg receiwed
		if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
		{
			
			if (receivedData.To == MODULEID)
			{
				blink(receivedData.Command, 500); // msg receiwed
				Serial.print("receivedData.From 	");
				Serial.println(receivedData.From);
				Serial.print("receivedData.To 	");
				Serial.println(receivedData.To);
				Serial.print("receivedData.Command	");
				Serial.println();
				Serial.print("receivedData.Data0	");
				Serial.println(receivedData.Data0);
				Serial.print("receivedData.Data1	");
				Serial.println(receivedData.Data1);
			}
			

			sendingData.To = receivedData.From; // just to scan network of devices
			sendingData.Command = receivedData.Command;
			sendingData.From = MODULEID;
			sendingData.Data0 = receivedData.Data0++;
			sendingData.Data1 = receivedData.Data1++;

			Serial.print("TX START: ");
			Serial.println(sizeof(sendingData));

			vw_send((uint8_t *)&receivedData, sizeof(sendingData));
			vw_wait_tx();
			Serial.println("STOP: ");

		}


	}
	else
	{
		blink(5, 30); // no msg receiwed
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
