#ifndef LISAKEYSLIB_H
#define LISAKEYSLIB_H

#define LK_INIT         0x80
#define LK_LAYOUT_FR    0xAD
#define LK_LAYOUT_DE    0xAE
#define LK_LAYOUT_UK    0xAF
#define LK_LAYOUT_US    0xBF
#define LK_FAIL         0xFF

#define LK_PRESSED      0x80
#define LK_RELEASED     0x00

#define LK_NUM_CLEAR    0x20
#define LK_NUM_MINUS    0x21
#define LK_NUM_PLUS     0x22
#define LK_NUM_TIMES    0x23
#define LK_NUM_7        0x24
#define LK_NUM_8        0x25
#define LK_NUM_9        0x26
#define LK_NUM_SLASH    0x27
#define LK_NUM_4        0x28
#define LK_NUM_5        0x29
#define LK_NUM_6        0x2A
#define LK_NUM_COMMA    0x2B
#define LK_NUM_PERIOD   0x2C
#define LK_NUM_2        0x2D
#define LK_NUM_3        0x2E
#define LK_NUM_ENTER    0x2F
#define LK_MINUS        0x40
#define LK_EQUAL        0x41
#define LK_BACKSLASH    0x42
#define LK_LESS_GREATER 0x43
#define LK_P            0x44
#define LK_BACKSPACE    0x45
#define LK_ENTER        0x46
#define LK_RETURN       0x48
#define LK_NUM_0        0x49
#define LK_SLASH        0x4C
#define LK_NUM_1        0x4D
#define LK_RT_OPTION    0x4E
#define LK_9            0x50
#define LK_0            0x51
#define LK_U            0x52
#define LK_I            0x53
#define LK_J            0x54
#define LK_K            0x55
#define LK_LT_BRACKET   0x56
#define LK_RT_BRACKET   0x57
#define LK_M            0x58
#define LK_L            0x59
#define LK_SEMICOLON    0x5A
#define LK_QUOTE        0x5B
#define LK_SPACE        0x5C
#define LK_COMMA        0x5D
#define LK_PERIOD       0x5E
#define LK_O            0x5F
#define LK_E            0x60
#define LK_6            0x61
#define LK_7            0x62
#define LK_8            0x63
#define LK_5            0x64
#define LK_R            0x65
#define LK_T            0x66
#define LK_Y            0x67
#define LK_TILDE        0x68
#define LK_F            0x69
#define LK_G            0x6A
#define LK_H            0x6B
#define LK_V            0x6C
#define LK_C            0x6D
#define LK_B            0x6E
#define LK_N            0x6F
#define LK_A            0x70
#define LK_2            0x71
#define LK_3            0x72
#define LK_4            0x73
#define LK_1            0x74
#define LK_Q            0x75
#define LK_S            0x76
#define LK_W            0x77
#define LK_TAB          0x78
#define LK_Z            0x79
#define LK_X            0x7A
#define LK_D            0x7B
#define LK_LT_OPTION    0x7C
#define LK_CAPS_LOCK    0x7D
#define LK_SHIFT        0x7E
#define LK_APPLE        0x7F

#define LK_MACRO_LTOPT  0x100
#define LK_MACRO_RTOPT  0x200
#define LK_MACRO_SHIFT  0x400
#define LK_MACRO_APPLE  0x800

#define LK_WRITE_DELAY  50

#define LK_FKEYS_NONE   0
#define LK_FKEYS_LISA   1
#define LK_FKEYS_MAC    2
#define LK_FKEYS_ZXCV   3

#define LK_BUFFER_SIZE  16

void lk_start_input();
unsigned char lk_read();

void lk_start_output(unsigned char layout);
void lk_write(unsigned char packet);
void lk_write_macro(unsigned int packet);
void lk_delay(unsigned long ms);
void lk_flush();

const char * lk_layout_name(unsigned char id);
const char * lk_key_name(unsigned char key);
char lk_key_to_ascii(unsigned char key, boolean shift);
unsigned char lk_key_to_usb(unsigned char key, boolean alt);
unsigned int lk_key_from_usb(unsigned char usb, unsigned char map);

#endif
