#ifndef KEYPADCONFIG_H
#define KEYPADCONFIG_H

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// Arduino Mega A0
#define KP_ANODE_0_DDR  DDRF
#define KP_ANODE_0_PORT PORTF
#define KP_ANODE_0_PIN  PINF
#define KP_ANODE_0_MASK 0x01

// Arduino Mega A1
#define KP_ANODE_1_DDR  DDRF
#define KP_ANODE_1_PORT PORTF
#define KP_ANODE_1_PIN  PINF
#define KP_ANODE_1_MASK 0x02

// Arduino Mega A2
#define KP_ANODE_2_DDR  DDRF
#define KP_ANODE_2_PORT PORTF
#define KP_ANODE_2_PIN  PINF
#define KP_ANODE_2_MASK 0x04

// Arduino Mega A3
#define KP_CATHODE_0_DDR  DDRF
#define KP_CATHODE_0_PORT PORTF
#define KP_CATHODE_0_PIN  PINF
#define KP_CATHODE_0_MASK 0x08

// Arduino Mega A4
#define KP_CATHODE_1_DDR  DDRF
#define KP_CATHODE_1_PORT PORTF
#define KP_CATHODE_1_PIN  PINF
#define KP_CATHODE_1_MASK 0x10

// Arduino Mega A5
#define KP_CATHODE_2_DDR  DDRF
#define KP_CATHODE_2_PORT PORTF
#define KP_CATHODE_2_PIN  PINF
#define KP_CATHODE_2_MASK 0x20

#else

// Arduino Uno A0
#define KP_ANODE_0_DDR  DDRC
#define KP_ANODE_0_PORT PORTC
#define KP_ANODE_0_PIN  PINC
#define KP_ANODE_0_MASK 0x01

// Arduino Uno A1
#define KP_ANODE_1_DDR  DDRC
#define KP_ANODE_1_PORT PORTC
#define KP_ANODE_1_PIN  PINC
#define KP_ANODE_1_MASK 0x02

// Arduino Uno A2
#define KP_ANODE_2_DDR  DDRC
#define KP_ANODE_2_PORT PORTC
#define KP_ANODE_2_PIN  PINC
#define KP_ANODE_2_MASK 0x04

// Arduino Uno A3
#define KP_CATHODE_0_DDR  DDRC
#define KP_CATHODE_0_PORT PORTC
#define KP_CATHODE_0_PIN  PINC
#define KP_CATHODE_0_MASK 0x08

// Arduino Uno A4
#define KP_CATHODE_1_DDR  DDRC
#define KP_CATHODE_1_PORT PORTC
#define KP_CATHODE_1_PIN  PINC
#define KP_CATHODE_1_MASK 0x10

// Arduino Uno A5
#define KP_CATHODE_2_DDR  DDRC
#define KP_CATHODE_2_PORT PORTC
#define KP_CATHODE_2_PIN  PINC
#define KP_CATHODE_2_MASK 0x20

#endif

#endif
