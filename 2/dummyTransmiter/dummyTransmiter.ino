#include <VirtualWire.h>
#define TX_MODULE                       10
#define RX_MODULE                       11

void setup() {

  Serial.begin(57600);  // Used to type in characters
  // put your setup code here, to run once:
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(TX_MODULE);
  vw_set_rx_pin(RX_MODULE);
  vw_setup(200);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Sending data");


  sendData();
  Serial.println("Sending data finshed");
  for (int i = 0; i < 10; i++) {

    receiveData();

  }
}

void receiveData()
{

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

 char message[30];

  if (vw_wait_rx_max(2000)) {
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      char tmp[1];

     String eoeo ;
      for (int i = 0; i < buflen; i++)
      {
         eoeo +=(char)buf[i]   ;
      }
     Serial.println("");    

          Serial.print("Got: ");       
          Serial.println(eoeo); 
 


    }
  }
  else{
      Serial.println("...");}


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
  char mesageBase[] = "00-01-00";
  char value [5];



  /////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////
  char mesage7[15];
  for (int i = 0; i < 15; i++)
  {
    mesage7[i] = 0;
  }

  strcat (mesage7, mesageBase);
  strcat (mesage7, "-");
  strcat (mesage7, "getAll");
  Serial.println(mesage7);
  //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
  strcat (mesage7, "_");
 const char *msg = mesage7;
  vw_send((uint8_t *)msg, strlen(mesage7));
  Serial.print("message sent:");
 Serial.println(strlen(mesage7));
  Serial.println(mesage7);
  vw_wait_tx(); // Wait until the whole message is gone
}
