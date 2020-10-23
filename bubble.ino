//File managing the bubble display - multiplexing, etc
//
// Implement printable interface
//
// char[12] human readable characters -> byte[12] 7-segment driver bits -> struct[12] port control register bits
// updated up to 50hz by interface    -> updated same as characters     -> updated by interrupt routine 2400 hz / 420 uS / 6.6k instruction cycles




//Default constructor
Bubble::Bubble()
{
  clear();

  //setup Timer2
  noInterrupts();
  // Clear registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // 1445.086705202312 Hz (16000000/((172+1)*64))
  OCR2A = 172;
  // CTC
  TCCR2A |= (1 << WGM21);
  // Prescaler 64
  TCCR2B |= (1 << CS22);
  // Output Compare Match A Interrupt Enable
  TIMSK2 |= (1 << OCIE2A);
  interrupts();
}

size_t Bubble::write(uint8_t chr)
{
  uint8_t bits =  pgm_read_byte(&asciiToSeven[chr]);

  segmentBits[cursor] = bits;

  updateBits(bits, cursor);

  cursor = ++cursor % SIZE;

  return 1;
}

bool Bubble::writeAt(uint8_t chr, uint8_t pos) {
  if (setCursor(pos)) {
    write(chr);
    return true;
  } else {
    return false;
  }
}


void Bubble::clear() { //clear display
  for (uint8_t i = 0; i < SIZE; i++) {
    segmentBits[i] = 0;
    updateBits(0, i);
  }
}

void Bubble::home() { //return to first line address 0
  cursor = 0;
}

bool Bubble::setCursor(uint8_t pos)
{
  if (pos < SIZE) {
    cursor = pos;
    return true;
  } else {
    return false;
  }
}

// Blink the digit to show cursor location
void Bubble::blinkOn() {
  blinkState = true;
}

// Stop the blinking
void Bubble::blinkOff() {
  blinkState = false;
}

void Bubble::setDecimal(uint8_t pos) {
  segmentBits[pos] |= DEC_BIT;
  updateBits(segmentBits[pos], pos);
}

void Bubble::clearDecimal(uint8_t pos) {
  segmentBits[pos] &= !DEC_BIT;
  updateBits(segmentBits[pos], pos);
}

//construct port driver bits and buffer them
void Bubble::updateBits(uint8_t bits, uint8_t pos) {

  //in cathode bank, one is low, others are high impedance
  //in two anode banks, high or high impedance according to bits

  switch (pos) {

    //Left display, cathodes on C
    case 0: case 1: case 2: case 3:
      levels[pos].b = directions[pos].b = bits & 0xf;

      directions[pos].c = 1 << pos;
      levels[pos].c = 0x00;

      levels[pos].d = directions[pos].d = bits & 0xf0;
      break;

    //Middle display, cathodes on B
    case 4: case 5: case 6: case 7:
      directions[pos].b = 1 << (pos - 4);
      levels[pos].b = 0x00;

      levels[pos].c = directions[pos].c = bits & 0x0f;

      levels[pos].d = directions[pos].d = bits & 0xf0;
      break;

    //Right display, cathodes on D
    case 8: case 9: case 10: case 11:
      levels[pos].b = directions[pos].b = (bits >> 4) & 0x0f;

      levels[pos].c = directions[pos].c = bits & 0x0f;

      directions[pos].d = 0b10000 << (pos - 8);
      levels[pos].d = 0x00;
      break;
  }
}

//interrupt routine calls this
inline void Bubble::scan1() {
  static uint8_t multiplexIndex = 0;
  multiplexIndex++;
  if (multiplexIndex >= SIZE) {
    multiplexIndex = 0;
  }

  //turn output drivers off - all high impedance, clear output bits
  //pins 8-11, PB0-PB3
  DDRB &= 0b11110000;
  PORTB &= 0b11110000;
  //pins A0-A3, PC0-PC3
  DDRC &= 0b11110000;
  PORTC &= 0b11110000;
  //pins 4-7, PD4-PD7
  DDRD &= 0b00001111;
  PORTD &= 0b00001111;

  //if meeting all the conditions to blink this digit off, return with things still off;
  if (blinkState && multiplexIndex == cursor && ((uint16_t)millis() & 256)) {
    return;
  }

  uint8_t bits = segmentBits[multiplexIndex];
  
  switch (multiplexIndex) {

    //Left display, cathodes on C
    case 0: case 1: case 2: case 3:
      PORTB |= bits & 0x0f;
      DDRB |= bits & 0x0f;

      DDRC |= 1 << multiplexIndex;

      PORTD |= bits & 0xf0;
      DDRD |= bits & 0xf0;
      break;

    //Middle display, cathodes on B
    case 4: case 5: case 6: case 7:
      DDRB |= 1 << (multiplexIndex - 4);

      DDRC |= bits & 0x0f;
      PORTC |= bits & 0x0f;

      DDRD |= bits & 0xf0;
      PORTD |= bits & 0xf0;
      break;

    //Right display, cathodes on D
    case 8: case 9: case 10: case 11:
      DDRB |= (bits >> 4) & 0x0f;
      PORTB |= (bits >> 4) & 0x0f;

      DDRC |= bits & 0x0f;
      PORTC |= bits & 0x0f;

      DDRD |= 0b10000 << (multiplexIndex - 8);
      break;
  }

}

//interrupt routine calls this
void Bubble::scan() {
  static uint8_t multiplexIndex = 0;
  multiplexIndex++;
  if (multiplexIndex >= SIZE) {
    multiplexIndex = 0;
  }

  //turn output drivers off - all high impedance
  //pins 8-11, PB0-PB3
  DDRB &= 0b11110000; //clear bits prior to ORing
  //pins A0-A3, PC0-PC3
  DDRC &= 0b11110000;
  //pins 4-7, PD4-PD7
  DDRD &= 0b00001111;

  //if meeting all the conditions to blink this digit off, return with things still off;
  if (blinkState && multiplexIndex == cursor && ((uint16_t)millis() & 256)) {
    return;
  }

  //set logic levels
  PORTB &= 0b11110000;
  PORTB |= levels[multiplexIndex].b;
  PORTC &= 0b11110000;
  PORTC |= levels[multiplexIndex].c;
  PORTD &= 0b00001111;
  PORTD |= levels[multiplexIndex].d;

  //turn selected output drivers back on
  DDRB |= directions[multiplexIndex].b;
  DDRC |= directions[multiplexIndex].c;
  DDRD |= directions[multiplexIndex].d;

}
