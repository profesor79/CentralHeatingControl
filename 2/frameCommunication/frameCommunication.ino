// receiver.pde
//
// Simple example of how to use VirtualWire to receive messages
// Implements a simplex (one-way) receiver with an Rx-B1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@open.com.au)
// Copyright (C) 2008 Mike McCauley
// $Id: receiver.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include <VirtualWire.h>

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



void setup()
{
  Serial.begin(9600);	// Debugging only
  Serial.println("setup");

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec

  vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;

    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");

    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    digitalWrite(13, false);
  }

  sendData();
}


void sendData()
{
  String from = "01-";
String to = "00-";
String command = "00-";
String insideTemp = "insideTemp:";
  //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
float insideTempValue=12.4567;

String insideTempValueString = String(insideTempValue, 2);   

String msg2 = from+to+command+insideTemp+insideTempValueString;
  char User [30] = "Logan";
  strcat (User, "'s Device");

char *msg =User  ;
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone

  delay(200);

}
