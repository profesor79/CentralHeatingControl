#include <avr/sleep.h>

long randNumber;
long randNumber2;
int wakePin = 9;                 // pin used for waking up
int sleepStatus = 0;             // variable to store a request for sleep
int count = 0;                   // counter
 

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
 
pinMode(wakePin, INPUT);
 
 
  /* Now it is time to enable an interrupt. In the function call
   * attachInterrupt(A, B, C)
   * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
   *
   * B   Name of a function you want to execute while in interrupt A.
   *
   * C   Trigger mode of the interrupt pin. can be:
   *             LOW        a low level trigger
   *             CHANGE     a change in level trigger
   *             RISING     a rising edge of a level trigger
   *             FALLING    a falling edge of a level trigger
   *
   * In all but the IDLE sleep modes only LOW can be used.
   */
 
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW
 
}

void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function\
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}
 


void wakeUpNow()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
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
    delay(2);
  }
randNumber=    random(1, 7);
randNumber2=    random(1, 7);
Serial.println(randNumber);
Serial.println(randNumber2);
digitalWrite(3,true);
digitalWrite(4,randNumber == 2 || randNumber > 2);
digitalWrite(5,randNumber == 3 || randNumber > 3);
digitalWrite(6,randNumber == 4 || randNumber > 4); //4
digitalWrite(7,randNumber == 5 || randNumber > 5);
digitalWrite(8,randNumber == 6 || randNumber > 6);


digitalWrite(17,true);
digitalWrite(16,randNumber2 == 2 || randNumber2 > 2);
digitalWrite(15,randNumber2 == 3 || randNumber2 > 3);
digitalWrite(14,randNumber2 == 4 || randNumber2 > 4); //4
digitalWrite(12,randNumber2 == 5 || randNumber2 > 5);
digitalWrite(11,randNumber2 == 6 || randNumber2 > 6);




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


  delay(100);     // this delay is needed, the sleep
                      //function will provoke a Serial error otherwise!!
 sleepNow();     // sleep function called here
}
