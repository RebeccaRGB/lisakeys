#include <Arduino.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lisakeysconfig.h"
#include "lisakeyslib.h"

void lk_start_input() {
  LK_DATA_DDR  |=  LK_DATA_MASK; // output
  LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  delay(100);
  LK_DATA_DDR  &=~ LK_DATA_MASK; // input
  LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  delay(100);
}

#define LK_PACKET_IN_SYNC  20
#define LK_PACKET_IN_START 23
#define LK_PACKET_IN_SHORT 15
#define LK_PACKET_IN_LONG1 23
#define LK_PACKET_IN_LONG2 23
#define LK_PACKET_IN_STOP  24

#define LK_PACKET_IN_MAX_DELAY  128
#define LK_PACKET_IN_MIN_PERIOD 768

unsigned char lk_read() {
  unsigned char packet = 0;
  noInterrupts();

  // Generate sync signal.
  LK_DATA_DDR  |=  LK_DATA_MASK; // output
  LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  _delay_us(LK_PACKET_IN_SYNC);
  LK_DATA_DDR  &=~ LK_DATA_MASK; // input
  LK_DATA_PORT |=  LK_DATA_MASK; // pull up

  // Wait for start bit.
  unsigned long time = micros();
  while (LK_DATA_PIN & LK_DATA_MASK) {
    if ((micros() - time) > LK_PACKET_IN_MAX_DELAY) {
      // If start bit doesn't come, no input.
      interrupts();
      _delay_us(LK_PACKET_IN_MIN_PERIOD);
      return 0;
    }
  }

  // Read data packet.
  _delay_us(LK_PACKET_IN_START);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x10;
  _delay_us(LK_PACKET_IN_SHORT);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x20;
  _delay_us(LK_PACKET_IN_SHORT);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x40;
  _delay_us(LK_PACKET_IN_LONG1);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x80;
  _delay_us(LK_PACKET_IN_LONG2);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x01;
  _delay_us(LK_PACKET_IN_SHORT);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x02;
  _delay_us(LK_PACKET_IN_SHORT);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x04;
  _delay_us(LK_PACKET_IN_SHORT);
  if (LK_DATA_PIN & LK_DATA_MASK) packet |= 0x08;
  _delay_us(LK_PACKET_IN_STOP);

  interrupts();
  _delay_us(LK_PACKET_IN_MIN_PERIOD);
  return ~packet;
}

static unsigned char layoutID;
static unsigned char keyBuffer[LK_BUFFER_SIZE];
static unsigned char keyBufStart;
static unsigned char keyBufEnd;

void lk_start_output(unsigned char layout) {
  LK_DATA_DDR  &=~ LK_DATA_MASK; // input
  LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  layoutID = layout;
  keyBuffer[0] = LK_INIT;
  keyBuffer[1] = layout;
  keyBufStart = 0;
  keyBufEnd = 2;
}

void lk_write(unsigned char packet) {
  keyBuffer[keyBufEnd] = packet;
  keyBufEnd++;
  keyBufEnd &= (LK_BUFFER_SIZE - 1);
  if (keyBufEnd == keyBufStart) {
    // Buffer overflowed. Ignore input.
    keyBufEnd--;
    keyBufEnd &= (LK_BUFFER_SIZE - 1);
  }
}

void lk_write_macro(unsigned int packet) {
  if (packet & 0x80) {
    if (packet & LK_MACRO_APPLE) {
      lk_write(LK_APPLE | LK_PRESSED);
      lk_delay(LK_WRITE_DELAY);
    }
    if (packet & LK_MACRO_LTOPT) {
      lk_write(LK_LT_OPTION | LK_PRESSED);
      lk_delay(LK_WRITE_DELAY);
    }
    if (packet & LK_MACRO_RTOPT) {
      lk_write(LK_RT_OPTION | LK_PRESSED);
      lk_delay(LK_WRITE_DELAY);
    }
    if (packet & LK_MACRO_SHIFT) {
      lk_write(LK_SHIFT | LK_PRESSED);
      lk_delay(LK_WRITE_DELAY);
    }
  }
  lk_write(packet & 0xFF);
  if (!(packet & 0x80)) {
    if (packet & LK_MACRO_SHIFT) {
      lk_delay(LK_WRITE_DELAY);
      lk_write(LK_SHIFT | LK_RELEASED);
    }
    if (packet & LK_MACRO_RTOPT) {
      lk_delay(LK_WRITE_DELAY);
      lk_write(LK_RT_OPTION | LK_RELEASED);
    }
    if (packet & LK_MACRO_LTOPT) {
      lk_delay(LK_WRITE_DELAY);
      lk_write(LK_LT_OPTION | LK_RELEASED);
    }
    if (packet & LK_MACRO_APPLE) {
      lk_delay(LK_WRITE_DELAY);
      lk_write(LK_APPLE | LK_RELEASED);
    }
  }
}

void lk_delay(unsigned long ms) {
  unsigned long time = millis();
  while ((millis() - time) < ms) {
    lk_flush();
  }
}

// Nominally 12?, 16, 16, 32, 16, but have to be adjusted
// to compensate for time taken by the microcontroller.
// These have been adjusted to work with a real Lisa.
#define LK_PACKET_OUT_DELAY 10
#define LK_PACKET_OUT_START 15
#define LK_PACKET_OUT_SHORT 15
#define LK_PACKET_OUT_LONG  30
#define LK_PACKET_OUT_STOP  15

void lk_flush() {
  // Wait for start of sync signal.
  unsigned long time = micros();
  while (LK_DATA_PIN & LK_DATA_MASK) {
    if ((micros() - time) > 4096) {
      // If start of sync doesn't come, try later.
      return;
    }
  }

  // Wait for end of sync signal.
  time = micros();
  while (!(LK_DATA_PIN & LK_DATA_MASK));
  if ((micros() - time) > 128) {
    // If too long to be a sync signal, reset.
    keyBuffer[0] = LK_INIT;
    keyBuffer[1] = layoutID;
    keyBufStart = 0;
    keyBufEnd = 2;
    return;
  }

  // If no output, don't respond.
  if (keyBufStart == keyBufEnd) return;

  noInterrupts();

  // Write data packet.
  unsigned char packet = keyBuffer[keyBufStart];
  keyBufStart++;
  keyBufStart &= (LK_BUFFER_SIZE - 1);
  _delay_us(LK_PACKET_OUT_DELAY);

  // start bit
  LK_DATA_DDR  |=  LK_DATA_MASK; // output
  LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  _delay_us(LK_PACKET_OUT_START);

  // bit 4
  if (packet & 0x10) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 5
  if (packet & 0x20) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 6
  if (packet & 0x40) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 7
  if (packet & 0x80) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_LONG);

  // bit 0
  if (packet & 0x01) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 1
  if (packet & 0x02) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 2
  if (packet & 0x04) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // bit 3
  if (packet & 0x08) LK_DATA_PORT &=~ LK_DATA_MASK; // pull down
  else               LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_SHORT);

  // stop bit
  LK_DATA_DDR  &=~ LK_DATA_MASK; // input
  LK_DATA_PORT |=  LK_DATA_MASK; // pull up
  _delay_us(LK_PACKET_OUT_STOP);

  interrupts();
}

const char * lk_layout_name(unsigned char id) {
  if (id == LK_LAYOUT_US) return "US";
  if (id == LK_LAYOUT_UK) return "UK";
  if (id == LK_LAYOUT_DE) return "DE";
  if (id == LK_LAYOUT_FR) return "FR";
  return "???";
}

static const char * PROGMEM numKeys[] = {
  "N Clear", "N -", "N +", "N *", "N 7", "N 8", "N 9", "N /",
  "N 4",     "N 5", "N 6", "N ,", "N .", "N 2", "N 3", "N Enter"
};

static const char * PROGMEM mainKeys[] = {
  "-",      "=",   "\\",  "<>",  "P",        "Backspace", "Enter",    "???",
  "Return", "N 0", "???", "???", "/",        "N 1",       "R Option", "???",
  "9",      "0",   "U",   "I",   "J",        "K",         "[",        "]",
  "M",      "L",   ";",   "\"",  "Space",    ",",         ".",        "O",
  "E",      "6",   "7",   "8",   "5",        "R",         "T",        "Y",
  "~",      "F",   "G",   "H",   "V",        "C",         "B",        "N",
  "A",      "2",   "3",   "4",   "1",        "Q",         "S",        "W",
  "Tab",    "Z",   "X",   "D",   "L Option", "Caps Lock", "Shift",    "Apple"
};

const char * lk_key_name(unsigned char key) {
  if (key & 0x40) return mainKeys[key & 0x3F];
  if ((key & 0x30) == 0x20) return numKeys[key & 0x0F];
  return "???";
}

static const char PROGMEM numKeysAscii[] = {
  0x1B, '-',  '+',  '*',  '7',  '8',  '9',  '/',
  '4',  '5',  '6',  ',',  '.',  '2',  '3',  0x0A
};

static const char PROGMEM mainKeysAscii[] = {
  '-',  '=',  '\\', '<',  'p',  0x08, 0x0A, 0,
  0x0A, '0',  0,    0,    '/',  '1',  0,    0,
  '9',  '0',  'u',  'i',  'j',  'k',  '[',  ']',
  'm',  'l',  ';',  '\'', ' ',  ',',  '.',  'o',
  'e',  '6',  '7',  '8',  '5',  'r',  't',  'y',
  '`',  'f',  'g',  'h',  'v',  'c',  'b',  'n',
  'a',  '2',  '3',  '4',  '1',  'q',  's',  'w',
  0x09, 'z',  'x',  'd',  0,    0,    0,    0
};

static const char PROGMEM mainKeysAsciiShift[] = {
  '_',  '+',  '|',  '>',  'P',  0x08, 0x0A, 0,
  0x0A, '0',  0,    0,    '?',  '1',  0,    0,
  '(',  ')',  'U',  'I',  'J',  'K',  '{',  '}',
  'M',  'L',  ':',  '"',  ' ',  '<',  '>',  'O',
  'E',  '^',  '&',  '*',  '%',  'R',  'T',  'Y',
  '~',  'F',  'G',  'H',  'V',  'C',  'B',  'N',
  'A',  '@',  '#',  '$',  '!',  'Q',  'S',  'W',
  0x09, 'Z',  'X',  'D',  0,    0,    0,    0
};

char lk_key_to_ascii(unsigned char key, boolean shift) {
  if (key & 0x40) {
    if (shift) return pgm_read_byte(&mainKeysAsciiShift[key & 0x3F]);
    else       return pgm_read_byte(&mainKeysAscii     [key & 0x3F]);
  } else if ((key & 0x30) == 0x20) {
    return pgm_read_byte(&numKeysAscii[key & 0x0F]);
  } else {
    return 0;
  }
}

static const unsigned char PROGMEM numKeysUsb[] = {
  0x53, 0x56, 0x57, 0x55, 0x5F, 0x60, 0x61, 0x54,
  0x5C, 0x5D, 0x5E, 0x85, 0x63, 0x5A, 0x5B, 0x58,
};

static const unsigned char PROGMEM mainKeysUsb[] = {
  0x2D, 0x2E, 0x31, 0x32, 0x13, 0x2A, 0xE7, 0x00,
  0x28, 0x62, 0x00, 0x00, 0x38, 0x59, 0xE6, 0x00,
  0x26, 0x27, 0x18, 0x0C, 0x0D, 0x0E, 0x2F, 0x30,
  0x10, 0x0F, 0x33, 0x34, 0x2C, 0x36, 0x37, 0x12,
  0x08, 0x23, 0x24, 0x25, 0x22, 0x15, 0x17, 0x1C,
  0x35, 0x09, 0x0A, 0x0B, 0x19, 0x06, 0x05, 0x11,
  0x04, 0x1F, 0x20, 0x21, 0x1E, 0x14, 0x16, 0x1A,
  0x2B, 0x1D, 0x1B, 0x07, 0xE2, 0x39, 0xE1, 0xE3,
};

// The alternate mapping makes the following changes to the numeric keypad:
//
//     NC -> PrtSc     N- -> ScrLk     N+ -> Left      N* -> Right
//     N7 -> Home      N8 -> Up        N9 -> PgUp      N/ -> Up
//     N4 -> Left      N5 -> Pause     N6 -> Right     N, -> Down
//     N1 -> End       N2 -> Down      N3 -> PgDn
//     N0 -> Ins                       N. -> Del

static const unsigned char PROGMEM numKeysUsbAlt[] = {
  0x46, 0x47, 0x50, 0x4F, 0x4A, 0x52, 0x4B, 0x52,
  0x50, 0x48, 0x4F, 0x51, 0x4C, 0x51, 0x4E, 0x58,
};

// The alternate mapping makes the following changes to the main key area:
//
//   ~ -> Esc   1-9 -> F1-F9   0 -> F10   - -> F11   = -> F12   Bksp -> Power
// Tab -> Menu                            [ -> Vol-  ] -> Vol+     \ -> Mute
//                                                            Return -> Eject
//
//            Apple -> LCtrl                     Enter -> RCtrl

static const unsigned char PROGMEM mainKeysUsbAlt[] = {
  0x44, 0x45, 0x7F, 0x64, 0x13, 0x66, 0xE4, 0x00,
  0xEC, 0x49, 0x00, 0x00, 0x38, 0x4D, 0xE6, 0x00,
  0x42, 0x43, 0x18, 0x0C, 0x0D, 0x0E, 0x81, 0x80,
  0x10, 0x0F, 0x33, 0x34, 0x2C, 0x36, 0x37, 0x12,
  0x08, 0x3F, 0x40, 0x41, 0x3E, 0x15, 0x17, 0x1C,
  0x29, 0x09, 0x0A, 0x0B, 0x19, 0x06, 0x05, 0x11,
  0x04, 0x3B, 0x3C, 0x3D, 0x3A, 0x14, 0x16, 0x1A,
  0x65, 0x1D, 0x1B, 0x07, 0xE2, 0x39, 0xE1, 0xE0,
};

unsigned char lk_key_to_usb(unsigned char key, boolean alt) {
  if (alt) {
    if (key & 0x40) return pgm_read_byte(&mainKeysUsbAlt[key & 0x3F]);
    if ((key & 0x30) == 0x20) return pgm_read_byte(&numKeysUsbAlt[key & 0x0F]);
  } else {
    if (key & 0x40) return pgm_read_byte(&mainKeysUsb[key & 0x3F]);
    if ((key & 0x30) == 0x20) return pgm_read_byte(&numKeysUsb[key & 0x0F]);
  }
  return 0;
}

static const unsigned int PROGMEM keyFromUsb[] = {
  0, 0, 0, 0, LK_A, LK_B, LK_C, LK_D, LK_E, LK_F, LK_G, LK_H,
  LK_I, LK_J, LK_K, LK_L, LK_M, LK_N, LK_O, LK_P, LK_Q, LK_R,
  LK_S, LK_T, LK_U, LK_V, LK_W, LK_X, LK_Y, LK_Z, LK_1, LK_2,
  LK_3, LK_4, LK_5, LK_6, LK_7, LK_8, LK_9, LK_0, LK_RETURN,

  // Esc
  (LK_MACRO_APPLE | LK_PERIOD),

  LK_BACKSPACE, LK_TAB, LK_SPACE, LK_MINUS, LK_EQUAL,
  LK_LT_BRACKET, LK_RT_BRACKET, LK_BACKSLASH,
  LK_LESS_GREATER, LK_SEMICOLON, LK_QUOTE, LK_TILDE,
  LK_COMMA, LK_PERIOD, LK_SLASH, LK_CAPS_LOCK,

  // F1 - F12
  -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12,

  // PrtSc, ScrLk, Pause
  -13, -14, -15,

  // Ins, Home, PgUp, Del, End, PgDn
  LK_ENTER, LK_TILDE, LK_BACKSLASH,
  LK_BACKSPACE, LK_LESS_GREATER, LK_NUM_COMMA,

  // Right, Left, Down, Up
  (LK_MACRO_APPLE | LK_NUM_TIMES),
  (LK_MACRO_APPLE | LK_NUM_PLUS),
  (LK_MACRO_APPLE | LK_NUM_COMMA),
  (LK_MACRO_APPLE | LK_NUM_SLASH),

  LK_NUM_CLEAR, LK_NUM_SLASH, LK_NUM_TIMES, LK_NUM_MINUS,
  LK_NUM_PLUS, LK_NUM_ENTER, LK_NUM_1, LK_NUM_2, LK_NUM_3,
  LK_NUM_4, LK_NUM_5, LK_NUM_6, LK_NUM_7, LK_NUM_8, LK_NUM_9,
  LK_NUM_0, LK_NUM_PERIOD, LK_BACKSLASH,

  // Menu, Power, Keypad =
  LK_ENTER, 0, LK_NUM_COMMA
};

static const unsigned int PROGMEM fkeysLisa[] = {
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_1    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_2    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_3    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_4    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_5    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_6    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_7    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_8    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_9    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_0    ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_MINUS),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_PLUS ),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_TIMES),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_SLASH),
  (LK_MACRO_RTOPT | LK_MACRO_SHIFT | LK_NUM_COMMA),
};

static const unsigned int PROGMEM fkeysMac[] = {
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_1         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_2         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_3         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_4         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_5         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_6         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_7         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_8         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_9         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_0         ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_MINUS     ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_EQUAL     ),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_LT_BRACKET),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_RT_BRACKET),
  (LK_MACRO_APPLE | LK_MACRO_SHIFT | LK_BACKSLASH ),
};

static const unsigned int PROGMEM fkeysZXCV[] = {
  (LK_MACRO_APPLE | LK_Z), // Undo
  (LK_MACRO_APPLE | LK_X), // Cut
  (LK_MACRO_APPLE | LK_C), // Copy
  (LK_MACRO_APPLE | LK_V), // Paste
  (LK_MACRO_APPLE | LK_N), // New
  (LK_MACRO_APPLE | LK_O), // Open
  (LK_MACRO_APPLE | LK_S), // Save
  (LK_MACRO_APPLE | LK_P), // Print
  (LK_MACRO_APPLE | LK_T), // Text Style
  (LK_MACRO_APPLE | LK_B), // Bold
  (LK_MACRO_APPLE | LK_I), // Italic
  (LK_MACRO_APPLE | LK_U), // Underline
  (LK_MACRO_APPLE | LK_F),
  (LK_MACRO_APPLE | LK_G),
  (LK_MACRO_APPLE | LK_H),
};

unsigned int lk_key_from_usb(unsigned char usb, unsigned char map) {
  if (usb < 0x68) {
    unsigned int packet = pgm_read_word(&keyFromUsb[usb]);
    if (packet & 0x8000) {
      switch (map & 0x03) {
        case LK_FKEYS_LISA: packet = pgm_read_word(&fkeysLisa[~packet]); break;
        case LK_FKEYS_MAC : packet = pgm_read_word(&fkeysMac [~packet]); break;
        case LK_FKEYS_ZXCV: packet = pgm_read_word(&fkeysZXCV[~packet]); break;
        default: packet = 0; break;
      }
    }
    return packet;
  }
  if (usb == 0x82) return LK_CAPS_LOCK;
  if (usb == 0x85) return LK_NUM_COMMA;
  return 0;
}
