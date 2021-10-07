#include <Arduino.h>
#include <avr/pgmspace.h>
#include "lisakeyslib.h"
#include "lisakeyswrite.h"

void lk_write_raw(unsigned char * s) {
  unsigned char ch;
  while ((ch = *s)) {
    lk_write(ch);
    lk_delay(LK_WRITE_DELAY);
    s++;
  }
}

static unsigned char digit_value(unsigned char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
  return 0xFF;
}

void lk_write_hex(unsigned char * s) {
  boolean inValue = false;
  unsigned char value;
  unsigned char ch;
  while ((ch = *s)) {
    if ((ch = digit_value(ch)) < 16) {
      if (inValue) {
        inValue = false;
        lk_write(value | ch);
        lk_delay(LK_WRITE_DELAY);
        value = 0;
      } else {
        inValue = true;
        value = ch << 4;
      }
    }
    s++;
  }
}

static const unsigned char PROGMEM keyFromAscii[] = {
  LK_SPACE,          LK_1|0x80,          LK_QUOTE|0x80,     LK_3|0x80,
  LK_4|0x80,         LK_5|0x80,          LK_7|0x80,         LK_QUOTE,
  LK_9|0x80,         LK_0|0x80,          LK_8|0x80,         LK_EQUAL|0x80,
  LK_COMMA,          LK_MINUS,           LK_PERIOD,         LK_SLASH,
  LK_0,              LK_1,               LK_2,              LK_3,
  LK_4,              LK_5,               LK_6,              LK_7,
  LK_8,              LK_9,               LK_SEMICOLON|0x80, LK_SEMICOLON,
  LK_COMMA|0x80,     LK_EQUAL,           LK_PERIOD|0x80,    LK_SLASH|0x80,
  LK_2|0x80,         LK_A|0x80,          LK_B|0x80,         LK_C|0x80,
  LK_D|0x80,         LK_E|0x80,          LK_F|0x80,         LK_G|0x80,
  LK_H|0x80,         LK_I|0x80,          LK_J|0x80,         LK_K|0x80,
  LK_L|0x80,         LK_M|0x80,          LK_N|0x80,         LK_O|0x80,
  LK_P|0x80,         LK_Q|0x80,          LK_R|0x80,         LK_S|0x80,
  LK_T|0x80,         LK_U|0x80,          LK_V|0x80,         LK_W|0x80,
  LK_X|0x80,         LK_Y|0x80,          LK_Z|0x80,         LK_LT_BRACKET,
  LK_BACKSLASH,      LK_RT_BRACKET,      LK_6|0x80,         LK_MINUS|0x80,
  LK_TILDE,          LK_A,               LK_B,              LK_C,
  LK_D,              LK_E,               LK_F,              LK_G,
  LK_H,              LK_I,               LK_J,              LK_K,
  LK_L,              LK_M,               LK_N,              LK_O,
  LK_P,              LK_Q,               LK_R,              LK_S,
  LK_T,              LK_U,               LK_V,              LK_W,
  LK_X,              LK_Y,               LK_Z,              LK_LT_BRACKET|0x80,
  LK_BACKSLASH|0x80, LK_RT_BRACKET|0x80, LK_TILDE|0x80,     LK_BACKSPACE
};

static unsigned char lk_key_from_ascii(unsigned char ch) {
  if (ch >= 0x20 && ch <= 0x7F) return pgm_read_byte(&keyFromAscii[ch - 0x20]);
  if (ch == 0x08) return LK_BACKSPACE;
  if (ch == 0x09) return LK_TAB;
  if (ch == 0x0A || ch == 0x0B) return LK_ENTER;
  if (ch == 0x0C || ch == 0x0D) return LK_RETURN;
  return 0;
}

void lk_write_ascii(unsigned char * s) {
  unsigned char shifted;
  unsigned char ch;
  while ((ch = *s)) {
    if ((ch = lk_key_from_ascii(ch))) {
      shifted = ch & 0x80;
      ch &= 0x7F;
      if (shifted) {
        lk_write(LK_SHIFT | LK_PRESSED);
        lk_delay(LK_WRITE_DELAY);
      }
      lk_write(ch | LK_PRESSED);
      lk_delay(LK_WRITE_DELAY);
      lk_write(ch | LK_RELEASED);
      lk_delay(LK_WRITE_DELAY);
      if (shifted) {
        lk_write(LK_SHIFT | LK_RELEASED);
        lk_delay(LK_WRITE_DELAY);
      }
    }
    s++;
  }
}

static const unsigned char PROGMEM codedValues[] = {
  LK_SPACE,        LK_NUM_1,      LK_QUOTE,      LK_NUM_3,
  LK_NUM_4,        LK_NUM_5,      0,             LK_QUOTE,
  0,               0,             LK_NUM_TIMES,  LK_NUM_PLUS,
  LK_COMMA,        LK_MINUS,      LK_PERIOD,     LK_SLASH,
  LK_0,            LK_1,          LK_2,          LK_3,
  LK_4,            LK_5,          LK_6,          LK_7,
  LK_8,            LK_9,          LK_SEMICOLON,  LK_SEMICOLON,
  LK_NUM_COMMA,    LK_EQUAL,      LK_NUM_PERIOD, LK_NUM_SLASH,
  LK_NUM_2,        LK_APPLE,      LK_BACKSPACE,  LK_CAPS_LOCK,
  LK_BACKSPACE,    LK_ENTER,      LK_NUM_ENTER,  LK_LESS_GREATER,
  LK_NUM_4,        LK_NUM_9,      LK_NUM_5,      LK_NUM_6,
  LK_LESS_GREATER, LK_ENTER,      LK_NUM_ENTER,  LK_LT_OPTION,
  LK_RT_OPTION,    LK_RT_OPTION,  LK_RETURN,     LK_SHIFT,
  LK_TAB,          LK_NUM_8,      LK_SPACE,      LK_SPACE,
  LK_NUM_CLEAR,    LK_NUM_7,      LK_NUM_0,      LK_LT_BRACKET,
  LK_BACKSLASH,    LK_RT_BRACKET, LK_NUM_6,      LK_NUM_MINUS,
  LK_TILDE,        LK_A,          LK_B,          LK_C,
  LK_D,            LK_E,          LK_F,          LK_G,
  LK_H,            LK_I,          LK_J,          LK_K,
  LK_L,            LK_M,          LK_N,          LK_O,
  LK_P,            LK_Q,          LK_R,          LK_S,
  LK_T,            LK_U,          LK_V,          LK_W,
  LK_X,            LK_Y,          LK_Z,          0,
  LK_SPACE,        0,             LK_TILDE,      0
};

void lk_write_coded(unsigned char * s) {
  boolean p = true;
  boolean r = true;
  unsigned char ch;
  while ((ch = *s)) {
    if (ch >= 0x20 && ch < 0x7F) {
      if (ch == '}' || ch == ')') {
        p = true;
        r = true;
      } else if (ch == '{') {
        p = true;
        r = false;
      } else if (ch == '(') {
        p = false;
        r = true;
      } else if ((ch = pgm_read_byte(&codedValues[ch - 0x20]))) {
        if (p) {
          lk_write(ch | LK_PRESSED);
          lk_delay(LK_WRITE_DELAY);
        }
        if (r) {
          lk_write(ch | LK_RELEASED);
          lk_delay(LK_WRITE_DELAY);
        }
      }
    }
    s++;
  }
}

void lk_write_blu_bootstrap(unsigned char s) {
  switch (s) {
    // Input Bootstrap
    case 0: lk_write_ascii(
      "2900 4dfa 0020 3456 227c 00fc d201 3c09 129e\r"
      "2910 66f4 0811 0000 67fa 14e9 0004 51ce fff4\r"
      "2920 4e75 09c0 0bd0 0444 0e01 03c1 05e8 0c00\r"
    ); break;
    // Set Bitrate
    case 1: lk_write_ascii("292e 0c00 0000\r"); break;
    case 2: lk_write_ascii("292e 0c01 0000\r"); break;
    case 3: lk_write_ascii("292e 0c04 0000\r"); break;
    case 4: lk_write_ascii("292e 0c0a 0000\r"); break;
    case 5: lk_write_ascii("292e 0c16 0000\r"); break;
    case 6: lk_write_ascii("292e 0c2e 0000\r"); break;
    case 7: lk_write_ascii("292e 0c5e 0000\r"); break;
    // Confirm Bootstrap
    case 8: lk_write_ascii("1900 30\r"); break;
    // Run Bootstrap
    case 9: lk_write_ascii("3900\r"); break;
    // Confirm Image
    case 10: lk_write_ascii("19c0 10\r1c14 10\r"); break;
    // Run Image
    case 11: lk_write_ascii("3c14\r"); break;
  }
}
