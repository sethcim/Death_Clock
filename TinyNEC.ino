//IR receiving using pin change interrupt instead of Timer2, to free up that resource.
//sets IRcode when it has a valid reception

byte IRpin = 2;

void setupTinyNEC() {
  pinMode(IRpin, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRpin), change, CHANGE);
}

//ISR
void change() {
  static uint32_t lastRise, width, data;
  static byte bits;
  //measure space width
  //if too long, reset
  //otherwise, shift in a 1 or 0
  //if data full, process and report
  //data encoded on space width, but reciever is active low

  if(digitalRead(IRpin)) {      //pin just went high (ir space)
    lastRise = micros();
  } else {                      //pin just went low (ir mark)
    width = micros()-lastRise;
    if(width > 2200) {          //timeout, or leading space
      data = 0;
      bits = 0;
    } else if(width > 1000) {   // a '1'
      bits++;
      data <<= 1;
      data |= 1;
    } else if(width > 250) {    // a '0'
      bits++;
      data <<= 1;
    } else {                    // some kind of garbage
      //reset???
      //data = 0;
      //bits = 0;
    }
    if(bits == 32) {
      bits = 0;
      byte code = (byte)(data >> 8);
      if(code == !(byte)data) {
        IRcode = code;
      }
    }
  }
}
