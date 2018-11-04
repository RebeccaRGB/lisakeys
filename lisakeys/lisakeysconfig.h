#ifndef LISAKEYSCONFIG_H
#define LISAKEYSCONFIG_H

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// Arduino Mega Digital Pin 6
#define LK_DATA_DDR  DDRH
#define LK_DATA_PORT PORTH
#define LK_DATA_PIN  PINH
#define LK_DATA_MASK 0x08

#else

// Arduino Uno Digital Pin 6
#define LK_DATA_DDR  DDRD
#define LK_DATA_PORT PORTD
#define LK_DATA_PIN  PIND
#define LK_DATA_MASK 0x40

#endif

#endif
