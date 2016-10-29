long randNumber;

void setup(){
  Serial.begin(9600);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
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
 pinMode(15, OUTPUT);
 pinMode(16, OUTPUT);
 
}

void loop() {
  

  // print a random number from 10 to 19
  randNumber = random(1, 7);
  Serial.println(randNumber);

  for (int t = 0; t < 50; t++) {

int i = t%6;
// turn the pin on:
    digitalWrite(i+3, HIGH);
    delay(50);
    // turn the pin off:
    digitalWrite(i+3, LOW);
  }


digitalWrite(3,true);
digitalWrite(4,randNumber == 2 || randNumber > 2);
digitalWrite(5,randNumber == 3 || randNumber > 3);
digitalWrite(6,randNumber == 4 || randNumber > 4); //4
digitalWrite(7,randNumber == 5 || randNumber > 5);
digitalWrite(8,randNumber == 6 || randNumber > 6);
delay(25000);

digitalWrite(3,0);
digitalWrite(4,0);
digitalWrite(5,0);
digitalWrite(6,0);
digitalWrite(7,0);
digitalWrite(8,0);

while(true)
{
}

}
