/*
* Waking Arduino up and putting it to sleep using two push buttons
*/

/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
//Deep Sleep
#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
/* _______________________________________Macros________________________________________ */
#define sleepPin 7  // When low, Arduino goes to sleep
#define wakePin 2   // when low, wakes Arduino up, must be an interrupt pin (2 or 3 on ATMEGA328P)
#define controlPin A0 

/* _____________________________Declarations_and_Variables______________________________ */
/* ______________________________________Functions______________________________________ */
// When wakePin is brought LOW this interrupt is triggered FIRST (even in PWR_DOWN sleep)
void sleepISR() {
  sleep_disable(); //Prevent sleep mode, so we don't enter it again, except deliberately, by code
  detachInterrupt(digitalPinToInterrupt(wakePin)); //Detach the interrupt that brought us out of sleep
  Serial.println("I'm awake now");
}
void putToSleep() {
  digitalWrite(controlPin, LOW);
  //Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19]) as these take up power
  static byte prevADCSRA = ADCSRA;
  ADCSRA = 0;
  /* 
  Set the type of sleep mode we want. Can be one of (in order of power saving):
  SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
  SLEEP_MODE_ADC
  SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
  SLEEP_MODE_EXT_STANDBY
  SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
  SLEEP_MODE_PWR_DOWN (Deep sleep) = Lowest Power Consumption
  */
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  /*
  Turn off Brown Out Detection (low voltage)
  What is it? A “brown out” of a microcontroller is a partial and temporary reduction in the 
  power supply voltage below the level required for reliable operation. 
  Many microcontrollers have a protection circuit which detects when the supply voltage goes below 
  this level and puts the device into a reset state to ensure proper startup when power returns.
  Brown Out Detection takes too much power so it is better to just disable it.
  Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
  BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets the MCU Control Register (MCUCR).
  Imp: The BODS bit is automatically cleared after three clock cycles.
  */
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS);
  /*
  Ensure we can wake up again by first disabling interupts (temporarily) so
  the wakeISR does not run before we are asleep and then prevent interrupts,
  and then defining the ISR (Interrupt Service Routine) to run when poked awake
  */  
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(wakePin), sleepISR, LOW);
  Serial.println("Good night! I'm about to sleep."); // Send a message just to show we are about to sleep
  Serial.flush();
  interrupts(); // Allow interrupts now
  sleep_cpu(); // And enter sleep mode as set above
}

/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin(9600);
  Serial.println("Inside Setup");
  // Keep pins high until we ground them
  pinMode(sleepPin, INPUT_PULLUP);
  pinMode(wakePin, INPUT_PULLUP);
  pinMode(controlPin, OUTPUT);
  digitalWrite(controlPin, HIGH);
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  Serial.println("I'm doing my work.");
  if(digitalRead(3) == LOW) {
    delay(2000);
    putToSleep();
  }
}
