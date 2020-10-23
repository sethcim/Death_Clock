// Bubble digit display is on pins 4-11 and A0-A3. 
// IR reciever on 2
// GPS on ?? overkill
// WIFI on ?? overkill
// 3, 12, 13, A4, A5 are available

//timed events
//display multiplexing (1440 hz on timer2 compare match)
//display incrementing (24-100 hz, pll timer1)
//cursor blinking (test on millis in ISR)
//ir reading (pin change interrupt)

//todo: test and fix tinyNEC
//todo: input modes state machine
//todo: wifi?
//todo: actual calculations and display

#include "bubble.h"

Bubble disp;
volatile uint8_t IRcode = 0;


void setup() {
  setupTinyNEC();
  disp.print("abcdefgh");
  //disp.blinkOn();
}

void loop() {
/*
  byte i = 11;
  do {
    disp.clear();
    disp.setCursor(i--);
    disp.print("Birth");
    delay(100);
  } while(i > 0);
  
  disp.clear();
  disp.setCursor(0);
  disp.print("Life years:");
  delay(3000);
  
  disp.clear();
  disp.setCursor(0);
  disp.write(16);
  disp.write(17);
  disp.print("onth");
  delay(3000);

  */

  disp.setCursor(0);
  disp.print("Jan Feb Mar"); //no march
  delay(3000);
  
  disp.setCursor(0);
  disp.print(" Apr May Jun"); //may
  delay(3000);
  
  disp.setCursor(0);
  disp.print("Jul Aug Sep");
  delay(3000);
  
  disp.setCursor(0);
  disp.print(" Oct Nou Dec"); //nov
  delay(3000);
    
  
  /*static int least = 493;
  if(++least > 9999) {
    least = 0;
  }
  disp.setCursor(8);
  if(least < 1000) disp.write(0);
  if(least < 100) disp.write(0);
  if(least < 10) disp.write(0);
  disp.print(least);
  disp.setCursor(9);
  delay(25);*/
}

void scroll(uint8_t text[], uint8_t len) {
  
}

void IRrecieve(uint8_t code) {
  
}

ISR(TIMER2_COMPA_vect) {
  disp.scan1();
}
