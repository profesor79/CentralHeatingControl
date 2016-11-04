#include <EEPROM.h>
#include <VirtualWire.h>



#define TX_MODULE 10
#define RX_MODULE 11
struct centralHeatingData
{
  int    From = 0;
  int    Seq = 1;
  int    To = -1;
  int    Command ;
  int    Ack;
  int    Data0 = 0;
  int    Data1 = 0;
  int    Data2 = 0;
  int    Data3 = 0;
  int    Data4 = 0;
  int    Data5 = 0;
  int    Data6 = 0;
  int    Data7 = 0;

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
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
}

void loop() {
  // put your main code here, to run repeatedly:
  // we have 10 devices
  for (int i = 1; i < 11; i++)
  {
    sendingData.To = i; // just to scan network of devices
    sendingData.Ack = i;
    sendingData.Command = i;


    Serial.print("Round: ");
    Serial.print(i);
    Serial.print("  START: ");
    Serial.println(sizeof(sendingData));
    vw_rx_stop();
    vw_send((uint8_t *)&receivedData, sizeof(sendingData));
    vw_wait_tx();
    vw_rx_start();       // Start the receiver PLL running
    Serial.println("STOP: ");

    uint8_t rcvdSize = sizeof(receivedData);
    //give a 5 tries to get ansver
    for (int y = 0; y < 105; y++) {
      Serial.print(y);
      Serial.print("...");
      if (vw_get_message((uint8_t *)&receivedData, &rcvdSize))
      {
        Serial.print("receivedData.From 	");
        Serial.println(receivedData.From 	);
        Serial.print("receivedData.Seq 	");
        Serial.println(receivedData.Seq 	);
        Serial.print("receivedData.To 	");
        Serial.println(receivedData.To 	);
        Serial.print("receivedData.Command	");
        Serial.println(receivedData.Command);
        Serial.print("receivedData.Ack	");
        Serial.println(receivedData.Ack	);
        Serial.print("receivedData.Data0	");
        Serial.println(receivedData.Data0	);
        Serial.print("receivedData.Data1	");
        Serial.println(receivedData.Data1	);
        Serial.print("receivedData.Data2	");
        Serial.println(receivedData.Data2	);
        Serial.print("receivedData.Data3 	");
        Serial.println(receivedData.Data3 	);
        Serial.print("receivedData.Data4	");
        Serial.println(receivedData.Data4	);
        Serial.print("receivedData.Data5	");
        Serial.println(receivedData.Data5	);
        Serial.print("receivedData.Data6	");
        Serial.println(receivedData.Data6	);
        Serial.print("receivedData.Data7	");
        Serial.println(receivedData.Data7	);
      }

      delay(10); //wait
    }
    Serial.println("...");
  }
}
