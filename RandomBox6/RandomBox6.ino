#include <avr/sleep.h>

long randNumber;

void setup(){
  Serial.begin(9600);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
 randomSeed(analogRead(7));
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
 pinMode(8, OUTPUT);
 pinMode(10, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(12, OUTPUT);
 pinMode(13, OUTPUT);
 pinMode(14, OUTPUT);
 pinMode(15, OUTPUT);
 pinMode(16, OUTPUT);
 pinMode(17, OUTPUT);
 
}

void loop() {
  
int ledPins[] {3,4,5,6,7,8,11,12,14,15,16,17};
  // print a random number from 10 to 19
  

int x = random(5, 15);
int y = 15 + x;
  for (int t = 0; t <y ; t++) {

int i = t%12;
// turn the pin on:
    digitalWrite(ledPins[i], HIGH);
    digitalWrite(13, HIGH);
    delay(50);
    // turn the pin off:
    digitalWrite(ledPins[i], LOW);
    digitalWrite(13, LOW);
    delay(10);
  }
randNumber=    random(1, 7);
Serial.println(randNumber);
digitalWrite(3,true);
digitalWrite(4,randNumber == 2 || randNumber > 2);
digitalWrite(5,randNumber == 3 || randNumber > 3);
digitalWrite(6,randNumber == 4 || randNumber > 4); //4
digitalWrite(7,randNumber == 5 || randNumber > 5);
digitalWrite(8,randNumber == 6 || randNumber > 6);


randNumber=    random(1, 7);
Serial.println(randNumber);

digitalWrite(11,true);
digitalWrite(12,randNumber == 2 || randNumber > 2);
digitalWrite(14,randNumber == 3 || randNumber > 3);
digitalWrite(15,randNumber == 4 || randNumber > 4); //4
digitalWrite(16,randNumber == 5 || randNumber > 5);
digitalWrite(17,randNumber == 6 || randNumber > 6);




delay(25000);
digitalWrite(3,0);
digitalWrite(4,0);
digitalWrite(5,0);
digitalWrite(6,0);
digitalWrite(7,0);
digitalWrite(8,0);

digitalWrite(11,0);
digitalWrite(12,0);
digitalWrite(14,0);
digitalWrite(15,0);
digitalWrite(16,0);
digitalWrite(17,0);


set_sleep_mode(SLEEP_MODE_PWR_DOWN);
sleep_enable();
sleep_cpu();

while(true)
{
}

}
