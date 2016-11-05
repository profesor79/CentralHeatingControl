#include <EEPROM.h>
#include <VirtualWire.h>



#define TX_MODULE 10
#define RX_MODULE 11
struct centralHeatingData
{
  uint8_t    From = 0;
  uint8_t    To = 255;
  uint8_t    Command = 0;
  uint8_t    Data0 = 0;
  uint8_t    Data1 = 0;
};

centralHeatingData receivedData;
centralHeatingData sendingData;


void setup() {
  // put your setup code here, to run once:
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
  // put your main code here, to run repeatedly:
  // we have 10 devices
  for (int i = 1; i < 11; i++)
  {
    sendingData.To = i; // just to scan network of devices
    sendingData.Command = i;
    sendingData.From = 0;
    sendingData.Data0 = 0;
    sendingData.Data1 = 0;

    Serial.print("Round: ");
    Serial.print(i);
    Serial.print("  START: ");
    Serial.println(sizeof(sendingData));

    vw_send((uint8_t *)&receivedData, sizeof(sendingData));
    vw_wait_tx();

    Serial.println("STOP: ");

    uint8_t rcvdSize = sizeof(receivedData);
    //give a 5 tries to get ansver
    for (int y = 0; y < 105; y++) {
   
      if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
      {
        Serial.print("receivedData.From 	");
        Serial.println(receivedData.From 	);
        Serial.print("receivedData.To 	");
        Serial.println(receivedData.To 	);
        Serial.print("receivedData.Command	");
        Serial.println(receivedData.Command);
        Serial.print("receivedData.Data0	");
        Serial.println(receivedData.Data0	);
        Serial.print("receivedData.Data1	");
        Serial.println(receivedData.Data1	);
      }

      delay(10); //wait
    }
    Serial.println("...");
  }
}
