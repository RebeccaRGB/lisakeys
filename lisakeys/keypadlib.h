#ifndef KEYPADLIB_H
#define KEYPADLIB_H

#define KP_ANODE_0     0x111
#define KP_ANODE_1     0x222
#define KP_ANODE_2     0x444

#define KP_CATHODE_0   0x00F
#define KP_CATHODE_1   0x0F0
#define KP_CATHODE_2   0xF00

#define KP_SW_1        (KP_ANODE_0 & KP_CATHODE_0)
#define KP_SW_2        (KP_ANODE_0 & KP_CATHODE_1)
#define KP_SW_3        (KP_ANODE_0 & KP_CATHODE_2)
#define KP_SW_S        (KP_ANODE_1 & KP_CATHODE_0)
#define KP_SW_PERIOD   (KP_ANODE_1 & KP_CATHODE_1)
#define KP_SW_RETURN   (KP_ANODE_1 & KP_CATHODE_2)
#define KP_SW_SHIFT    (KP_ANODE_2 & KP_CATHODE_0)
#define KP_SW_APPLE    (KP_ANODE_2 & KP_CATHODE_1)
#define KP_SW_SPACE    (KP_ANODE_2 & KP_CATHODE_2)

#define KP_BUFFER_SIZE 16

void keypad_init();
int keypad_read();
unsigned char keypad_read_packet();

#endif
