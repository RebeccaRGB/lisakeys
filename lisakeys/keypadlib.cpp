#include <Arduino.h>
#include <util/delay.h>
#include "keypadconfig.h"
#include "keypadlib.h"
#include "lisakeyslib.h"

void keypad_init() {
  // Anodes are pulled high and read as inputs.
  KP_ANODE_0_DDR  &=~ KP_ANODE_0_MASK;
  KP_ANODE_0_PORT |=  KP_ANODE_0_MASK;
  KP_ANODE_1_DDR  &=~ KP_ANODE_1_MASK;
  KP_ANODE_1_PORT |=  KP_ANODE_1_MASK;
  KP_ANODE_2_DDR  &=~ KP_ANODE_2_MASK;
  KP_ANODE_2_PORT |=  KP_ANODE_2_MASK;
  // Cathodes are normally high but pulled low when probing.
  KP_CATHODE_0_DDR  |= KP_CATHODE_0_MASK;
  KP_CATHODE_0_PORT |= KP_CATHODE_0_MASK;
  KP_CATHODE_1_DDR  |= KP_CATHODE_1_MASK;
  KP_CATHODE_1_PORT |= KP_CATHODE_1_MASK;
  KP_CATHODE_2_DDR  |= KP_CATHODE_2_MASK;
  KP_CATHODE_2_PORT |= KP_CATHODE_2_MASK;
}

int keypad_read() {
  int state = 0;

  KP_CATHODE_0_PORT &=~ KP_CATHODE_0_MASK;
  _delay_us(16);
  if (!(KP_ANODE_0_PIN & KP_ANODE_0_MASK)) state |= (KP_ANODE_0 & KP_CATHODE_0);
  if (!(KP_ANODE_1_PIN & KP_ANODE_1_MASK)) state |= (KP_ANODE_1 & KP_CATHODE_0);
  if (!(KP_ANODE_2_PIN & KP_ANODE_2_MASK)) state |= (KP_ANODE_2 & KP_CATHODE_0);
  KP_CATHODE_0_PORT |=  KP_CATHODE_0_MASK;

  KP_CATHODE_1_PORT &=~ KP_CATHODE_1_MASK;
  _delay_us(16);
  if (!(KP_ANODE_0_PIN & KP_ANODE_0_MASK)) state |= (KP_ANODE_0 & KP_CATHODE_1);
  if (!(KP_ANODE_1_PIN & KP_ANODE_1_MASK)) state |= (KP_ANODE_1 & KP_CATHODE_1);
  if (!(KP_ANODE_2_PIN & KP_ANODE_2_MASK)) state |= (KP_ANODE_2 & KP_CATHODE_1);
  KP_CATHODE_1_PORT |=  KP_CATHODE_1_MASK;

  KP_CATHODE_2_PORT &=~ KP_CATHODE_2_MASK;
  _delay_us(16);
  if (!(KP_ANODE_0_PIN & KP_ANODE_0_MASK)) state |= (KP_ANODE_0 & KP_CATHODE_2);
  if (!(KP_ANODE_1_PIN & KP_ANODE_1_MASK)) state |= (KP_ANODE_1 & KP_CATHODE_2);
  if (!(KP_ANODE_2_PIN & KP_ANODE_2_MASK)) state |= (KP_ANODE_2 & KP_CATHODE_2);
  KP_CATHODE_2_PORT |=  KP_CATHODE_2_MASK;

  return state;
}

static int lastState = 0;
static unsigned char keypadBuffer[KP_BUFFER_SIZE];
static unsigned char keypadBufStart = 0;
static unsigned char keypadBufEnd = 0;

static void keypad_write_packet(unsigned char packet) {
  keypadBuffer[keypadBufEnd] = packet;
  keypadBufEnd++;
  keypadBufEnd &= (KP_BUFFER_SIZE - 1);
  if (keypadBufEnd == keypadBufStart) {
    // Buffer overflowed. Ignore input.
    keypadBufEnd--;
    keypadBufEnd &= (KP_BUFFER_SIZE - 1);
  }
}

unsigned char keypad_read_packet() {
  unsigned char packet;
  int nextState = keypad_read();
  int r = lastState &~ nextState;
  int p = nextState &~ lastState;
  lastState = nextState;

  // If set in the old state but not in the new state, button was released.
  if (r & KP_SW_1     ) keypad_write_packet(LK_1      | LK_RELEASED);
  if (r & KP_SW_2     ) keypad_write_packet(LK_2      | LK_RELEASED);
  if (r & KP_SW_3     ) keypad_write_packet(LK_3      | LK_RELEASED);
  if (r & KP_SW_S     ) keypad_write_packet(LK_S      | LK_RELEASED);
  if (r & KP_SW_PERIOD) keypad_write_packet(LK_PERIOD | LK_RELEASED);
  if (r & KP_SW_RETURN) keypad_write_packet(LK_RETURN | LK_RELEASED);
  if (r & KP_SW_SHIFT ) keypad_write_packet(LK_SHIFT  | LK_RELEASED);
  if (r & KP_SW_APPLE ) keypad_write_packet(LK_APPLE  | LK_RELEASED);
  if (r & KP_SW_SPACE ) keypad_write_packet(LK_SPACE  | LK_RELEASED);

  // If set in the new state but not in the old state, button was pressed.
  if (p & KP_SW_1     ) keypad_write_packet(LK_1      | LK_PRESSED);
  if (p & KP_SW_2     ) keypad_write_packet(LK_2      | LK_PRESSED);
  if (p & KP_SW_3     ) keypad_write_packet(LK_3      | LK_PRESSED);
  if (p & KP_SW_S     ) keypad_write_packet(LK_S      | LK_PRESSED);
  if (p & KP_SW_PERIOD) keypad_write_packet(LK_PERIOD | LK_PRESSED);
  if (p & KP_SW_RETURN) keypad_write_packet(LK_RETURN | LK_PRESSED);
  if (p & KP_SW_SHIFT ) keypad_write_packet(LK_SHIFT  | LK_PRESSED);
  if (p & KP_SW_APPLE ) keypad_write_packet(LK_APPLE  | LK_PRESSED);
  if (p & KP_SW_SPACE ) keypad_write_packet(LK_SPACE  | LK_PRESSED);

  if (r || p) delay(100); // debounce

  if (keypadBufStart == keypadBufEnd) return 0;
  packet = keypadBuffer[keypadBufStart];
  keypadBufStart++;
  keypadBufStart &= (KP_BUFFER_SIZE - 1);
  return packet;
}
