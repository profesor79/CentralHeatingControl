#include "dht.h"
long tempDebounce = 3000;
long time2 = -30000;  // force temp reading



dht11 DHT11;

int humidity;
int temp;

void setup()
{

  /* add setup code here */
	Serial.begin(9600);

}

void loop()
{

  /* add main program code here */
	if (millis() - time2 > tempDebounce) {
		time2 = millis();
		int chk = DHT11.read(12);

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

}
