#ifndef USBKEYSLIB_H
#define USBKEYSLIB_H

#define USBK_NONE            0x00
#define USBK_OVERFLOW        0x01
#define USBK_POST_FAIL       0x02
#define USBK_UNDEFINED       0x03
#define USBK_A               0x04
#define USBK_B               0x05
#define USBK_C               0x06
#define USBK_D               0x07
#define USBK_E               0x08
#define USBK_F               0x09
#define USBK_G               0x0A
#define USBK_H               0x0B
#define USBK_I               0x0C
#define USBK_J               0x0D
#define USBK_K               0x0E
#define USBK_L               0x0F
#define USBK_M               0x10
#define USBK_N               0x11
#define USBK_O               0x12
#define USBK_P               0x13
#define USBK_Q               0x14
#define USBK_R               0x15
#define USBK_S               0x16
#define USBK_T               0x17
#define USBK_U               0x18
#define USBK_V               0x19
#define USBK_W               0x1A
#define USBK_X               0x1B
#define USBK_Y               0x1C
#define USBK_Z               0x1D
#define USBK_1               0x1E
#define USBK_2               0x1F
#define USBK_3               0x20
#define USBK_4               0x21
#define USBK_5               0x22
#define USBK_6               0x23
#define USBK_7               0x24
#define USBK_8               0x25
#define USBK_9               0x26
#define USBK_0               0x27
#define USBK_RETURN          0x28
#define USBK_ESC             0x29
#define USBK_BACKSPACE       0x2A
#define USBK_TAB             0x2B
#define USBK_SPACE           0x2C
#define USBK_MINUS           0x2D
#define USBK_EQUAL           0x2E
#define USBK_LT_BRACKET      0x2F
#define USBK_RT_BRACKET      0x30
#define USBK_BACKSLASH       0x31
#define USBK_POUND           0x32
#define USBK_SEMICOLON       0x33
#define USBK_QUOTE           0x34
#define USBK_TILDE           0x35
#define USBK_COMMA           0x36
#define USBK_PERIOD          0x37
#define USBK_SLASH           0x38
#define USBK_CAPS_LOCK       0x39
#define USBK_F1              0x3A
#define USBK_F2              0x3B
#define USBK_F3              0x3C
#define USBK_F4              0x3D
#define USBK_F5              0x3E
#define USBK_F6              0x3F
#define USBK_F7              0x40
#define USBK_F8              0x41
#define USBK_F9              0x42
#define USBK_F10             0x43
#define USBK_F11             0x44
#define USBK_F12             0x45
#define USBK_SYSRQ           0x46
#define USBK_SCRLK           0x47
#define USBK_PAUSE           0x48
#define USBK_INS             0x49
#define USBK_HOME            0x4A
#define USBK_PGUP            0x4B
#define USBK_DEL             0x4C
#define USBK_END             0x4D
#define USBK_PGDN            0x4E
#define USBK_RIGHT           0x4F
#define USBK_LEFT            0x50
#define USBK_DOWN            0x51
#define USBK_UP              0x52
#define USBK_NUM_CLEAR       0x53
#define USBK_NUM_SLASH       0x54
#define USBK_NUM_TIMES       0x55
#define USBK_NUM_MINUS       0x56
#define USBK_NUM_PLUS        0x57
#define USBK_NUM_ENTER       0x58
#define USBK_NUM_1           0x59
#define USBK_NUM_2           0x5A
#define USBK_NUM_3           0x5B
#define USBK_NUM_4           0x5C
#define USBK_NUM_5           0x5D
#define USBK_NUM_6           0x5E
#define USBK_NUM_7           0x5F
#define USBK_NUM_8           0x60
#define USBK_NUM_9           0x61
#define USBK_NUM_0           0x62
#define USBK_NUM_PERIOD      0x63
#define USBK_102ND           0x64
#define USBK_MENU            0x65
#define USBK_POWER           0x66
#define USBK_NUM_EQUAL       0x67

#define USBK_LT_CTRL         0xE0
#define USBK_LT_SHIFT        0xE1
#define USBK_LT_ALT          0xE2
#define USBK_LT_META         0xE3
#define USBK_RT_CTRL         0xE4
#define USBK_RT_SHIFT        0xE5
#define USBK_RT_ALT          0xE6
#define USBK_RT_META         0xE7

#define USBK_LED_NUM_LOCK    0x01
#define USBK_LED_CAPS_LOCK   0x02
#define USBK_LED_SCROLL_LOCK 0x04
#define USBK_LED_COMPOSE     0x08
#define USBK_LED_KANA        0x10

#define USBK_MOD_LT_CTRL     0x01
#define USBK_MOD_LT_SHIFT    0x02
#define USBK_MOD_LT_ALT      0x04
#define USBK_MOD_LT_META     0x08
#define USBK_MOD_RT_CTRL     0x10
#define USBK_MOD_RT_SHIFT    0x20
#define USBK_MOD_RT_ALT      0x40
#define USBK_MOD_RT_META     0x80
#define USBK_MOD_ANY_CTRL    0x11
#define USBK_MOD_ANY_SHIFT   0x22
#define USBK_MOD_ANY_ALT     0x44
#define USBK_MOD_ANY_META    0x88

#define USBK_EVENT_MOD_STATE 0x0200
#define USBK_EVENT_MOD_DOWN  0x0400
#define USBK_EVENT_MOD_UP    0x0800
#define USBK_EVENT_KEY_DOWN  0x1000
#define USBK_EVENT_KEY_UP    0x2000

#define USBK_BUFFER_SIZE     16

void usbk_start_output();
void usbk_key_pressed(unsigned char key);
void usbk_key_released(unsigned char key);
unsigned char usbk_read_leds();

void usbk_start_input();
unsigned int usbk_read_keys();
void usbk_write_leds(unsigned char leds);

#endif
