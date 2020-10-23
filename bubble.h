#define SIZE 12 //digits in display
#define DEC_BIT 0b10000000;

struct Ports {
  uint8_t b, c, d;
};

class Bubble: public Print 
{
  public:
    Bubble(); 
    void clear();
    void home();
    bool setCursor(uint8_t pos);
    void blinkOn();
    void blinkOff();
    void setDecimal(uint8_t pos);
    void clearDecimal(uint8_t pos);
    virtual size_t write(uint8_t chr);
    bool writeAt(uint8_t chr, uint8_t pos);
    const uint8_t size = SIZE;
    void scan();
    inline void scan1() __attribute__((always_inline));
  private:
    void updateBits(uint8_t bits, uint8_t pos);
    uint8_t segmentBits[SIZE];
    uint8_t cursor = 0;         // 0 is left most, increments from left to right
    bool blinkState = false;
    bool insertBlink = false;
    volatile Ports directions[SIZE];
    volatile Ports levels[SIZE];
};

//convert ASCII chars and hex bytes to 7 segment drive bits
const byte asciiToSeven[128] PROGMEM = {
  0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70,     //ascii 0 - 7
  0x7F, 0x7B, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47,     //ascii 8 - 15
  0x46, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //ascii 16 - 23
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //ascii 24 - 31
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //ascii 32 - 39
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x7E,     //ascii 40 - 48
  0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F,     //ascii 49 - 56: 1 through 8
  0x7B, 0x00, 0x00, 0x00, 0x09, 0x00, 0x65,     //ascii 57 - 64: 9 at 57, ? at 63
  0x00, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E,     //ascii 65 - 72: @ now printed at  A at 65 = 0x77
  0x37, 0x06, 0x3C, 0x00, 0x0E, 0x00, 0x15, 0x7E, 
  0x67, 0x73, 0x05, 0x5B, 0x0F, 0x3E, 0x00, 0x00, 
  0x00, 0x3B, 0x00, 0x4E, 0x00, 0x78, 0x00, 0x08, 
  0x00, 0x7D, 0x1F, 0x0D, 0x3D, 0x4F, 0x47, 0x5E, 
  0x17, 0x04, 0x3C, 0x00, 0x0E, 0x00, 0x15, 0x1D, 
  0x67, 0x73, 0x05, 0x5B, 0x0F, 0x1C, 0x00, 0x00, 
  0x00, 0x3B, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00
}; // result = pgm_read_byte(&asciiToSeven[input]);
