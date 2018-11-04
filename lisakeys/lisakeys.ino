#include <Arduino.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "config.h"
#include "keypadconfig.h"
#include "keypadlib.h"
#include "lisakeysconfig.h"
#include "lisakeyslib.h"
#include "lisakeyswrite.h"
#include "usbkeysconfig.h"
#include "usbkeyslib.h"

#ifdef USBK_USE_HOST_SHIELD
#include <hidboot.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#endif

void setup() {
  pinMode(LK_SW_HOST_CLIENT_MODE, INPUT_PULLUP);
  pinMode(LK_SW_SERIAL_USB_MODE, INPUT_PULLUP);
  if (digitalRead(LK_SW_HOST_CLIENT_MODE)) {
    if (digitalRead(LK_SW_SERIAL_USB_MODE)) {
      serial_input_mode();
    } else {
      usb_input_mode();
    }
  } else {
    if (digitalRead(LK_SW_SERIAL_USB_MODE)) {
      serial_output_mode();
    } else {
      usb_output_mode();
    }
  }
}

void loop() {}

void serial_input_mode() {
  boolean isIDByte = false;
  unsigned char packet;
  unsigned char pressed;
  keypad_init();
  lk_start_input();
  Serial.begin(9600);
  Serial.println("READY");
  while (true) {
    if ((packet = lk_read()) || (packet = keypad_read_packet())) {
      if (packet == LK_INIT) {
        isIDByte = true;
      } else if (isIDByte) {
        Serial.print("INITED ");
        Serial.print(packet, HEX);
        Serial.print(" ");
        Serial.println(lk_layout_name(packet));
        isIDByte = false;
      } else {
        pressed = packet & 0x80;
        packet &= 0x7F;
        Serial.print(pressed ? "PRESSED " : "RELEASED ");
        Serial.print(packet, HEX);
        Serial.print(" ");
        Serial.println(lk_key_name(packet));
      }
    }
  }
}

void usb_input_mode() {
  boolean isIDByte = false;
  unsigned char packet;
  unsigned char pressed;
  unsigned char shiftKey;
  unsigned char optionKey;
  unsigned char appleKey;
  unsigned char enterKey;
  boolean fnDown = false;
  pinMode(LK_LED, OUTPUT);
  pinMode(LK_SW_SETTING_1, INPUT_PULLUP);
  pinMode(LK_SW_SETTING_2, INPUT_PULLUP);
  digitalWrite(LK_LED, LOW);
  if (digitalRead(LK_SW_SETTING_1)) {
    // PC Layout
    shiftKey  = USBK_LT_SHIFT;
    optionKey = USBK_LT_CTRL;
    appleKey  = USBK_LT_ALT;
    enterKey  = digitalRead(LK_SW_SETTING_2)
              ? USBK_LT_META
              : USBK_RT_ALT;
  } else {
    // Mac Layout
    shiftKey  = USBK_LT_SHIFT;
    optionKey = USBK_LT_ALT;
    appleKey  = USBK_LT_META;
    enterKey  = digitalRead(LK_SW_SETTING_2)
              ? USBK_LT_CTRL
              : USBK_RT_META;
  }
  keypad_init();
  lk_start_input();
  usbk_start_output();
  while (true) {
    if ((packet = lk_read()) || (packet = keypad_read_packet())) {
      if (packet == LK_INIT) {
        isIDByte = true;
      } else if (isIDByte) {
        isIDByte = false;
      } else {
        pressed = packet & 0x80;
        packet &= 0x7F;
        if (packet == LK_RT_OPTION) {
          // Use Right Option as Fn key to input keys not on
          // keyboard. Number row becomes esc + function keys.
          // Numeric keypad becomes ins, del, arrow keys, etc.
          // See lisakeyslib.cpp for a complete list.
          fnDown = pressed;
          digitalWrite(LK_LED, pressed);
          continue;
        } else if (packet == LK_ENTER) {
          packet = enterKey;
        } else if (packet == LK_APPLE) {
          packet = appleKey;
        } else if (packet == LK_LT_OPTION) {
          packet = optionKey;
        } else if (packet == LK_SHIFT) {
          packet = shiftKey;
        } else if (packet == LK_CAPS_LOCK) {
          // Lisa's caps lock is locking, but USB's is momentary.
          // So send key down, wait, and send key up to toggle,
          // but only if LED state does not match keyboard state.
          packet = usbk_read_leds();
          if (!pressed != !(packet & USBK_LED_CAPS_LOCK)) {
            usbk_key_pressed(USBK_CAPS_LOCK);
            delay(50);
            usbk_key_released(USBK_CAPS_LOCK);
          }
          continue;
        } else {
          packet = lk_key_to_usb(packet, fnDown);
          if (!packet) continue;
        }
        if (pressed) usbk_key_pressed(packet);
        else usbk_key_released(packet);
      }
    }
  }
}

static unsigned char get_layout_setting() {
  pinMode(LK_SW_SETTING_1, INPUT_PULLUP);
  pinMode(LK_SW_SETTING_2, INPUT_PULLUP);
  if (digitalRead(LK_SW_SETTING_1)) {
    return digitalRead(LK_SW_SETTING_2)
         ? LK_LAYOUT_US : LK_LAYOUT_UK;
  } else {
    return digitalRead(LK_SW_SETTING_2)
         ? LK_LAYOUT_DE : LK_LAYOUT_FR;
  }
}

#define SERIAL_BUFFER_SIZE 80

void serial_output_mode() {
  unsigned char packet;
  unsigned char buf[SERIAL_BUFFER_SIZE];
  int ptr = 0;
  int ch;
  keypad_init();
  lk_start_output(get_layout_setting());
  Serial.begin(9600);
  Serial.println("READY");
  while (true) {
    if ((packet = keypad_read_packet())) {
      lk_write(packet);
    }
    if ((ch = Serial.read()) > 0) {
      if (ch == '\n' || ch == '\r') {
        buf[ptr] = 0;
        if      (buf[0] == '!') Serial.println(free_ram());
        else if (buf[0] == '#') /* "comment" / no-op */;
        else if (buf[0] == '$') lk_write_hex  (&buf[1]);
        else if (buf[0] == '%') lk_write_raw  (&buf[1]);
        else if (buf[0] == '&') lk_write_coded(&buf[1]);
        else if (buf[0] == '*') lk_write_blu_bootstrap(buf[1] & 0x0F);
        else if (buf[0] == '@') lk_write_ascii(&buf[1]);
        else                    lk_write_ascii(&buf[0]);
        ptr = 0;
      } else if (ptr < (SERIAL_BUFFER_SIZE - 1)) {
        buf[ptr] = ch;
        ptr++;
      }
    }
    lk_flush();
  }
}

void usb_output_mode() {
  unsigned char packet;
  unsigned char modState = 0;
  unsigned char ledState = 0;
  unsigned char lCtrlKey, lAltKey, lMetaKey;
  unsigned char rCtrlKey, rAltKey, rMetaKey;
  unsigned char fkeyMap;
  unsigned int e;
  pinMode(LK_SW_SETTING_3, INPUT_PULLUP);
  pinMode(LK_SW_SETTING_4, INPUT_PULLUP);
  if (digitalRead(LK_SW_SETTING_3)) {
    // PC Layout
    lCtrlKey = lMetaKey = LK_LT_OPTION; lAltKey = LK_APPLE;
    rCtrlKey = rMetaKey = LK_RT_OPTION; rAltKey = LK_ENTER;
  } else {
    // Mac Layout
    lAltKey = LK_LT_OPTION; lCtrlKey = lMetaKey = LK_APPLE;
    rAltKey = LK_RT_OPTION; rCtrlKey = rMetaKey = LK_ENTER;
  }
  if (digitalRead(LK_SW_SETTING_4)) {
    fkeyMap = LK_FKEYS_LISA;
  } else {
    fkeyMap = LK_FKEYS_MAC;
  }
  keypad_init();
  lk_start_output(get_layout_setting());
  usbk_start_input();
  while (true) {
    if ((packet = keypad_read_packet())) {
      lk_write(packet);
    }
    if ((e = usbk_read_keys())) {
      if (e & USBK_EVENT_MOD_STATE) {
        modState = e;
      } else if (e & USBK_EVENT_MOD_DOWN) {
        if (e & USBK_MOD_ANY_SHIFT) lk_write(LK_SHIFT | LK_PRESSED);
        if (e & USBK_MOD_LT_CTRL  ) lk_write(lCtrlKey | LK_PRESSED);
        if (e & USBK_MOD_LT_ALT   ) lk_write(lAltKey  | LK_PRESSED);
        if (e & USBK_MOD_LT_META  ) lk_write(lMetaKey | LK_PRESSED);
        if (e & USBK_MOD_RT_CTRL  ) lk_write(rCtrlKey | LK_PRESSED);
        if (e & USBK_MOD_RT_ALT   ) lk_write(rAltKey  | LK_PRESSED);
        if (e & USBK_MOD_RT_META  ) lk_write(rMetaKey | LK_PRESSED);
      } else if (e & USBK_EVENT_MOD_UP) {
        if (e & USBK_MOD_ANY_SHIFT) lk_write(LK_SHIFT | LK_RELEASED);
        if (e & USBK_MOD_LT_CTRL  ) lk_write(lCtrlKey | LK_RELEASED);
        if (e & USBK_MOD_LT_ALT   ) lk_write(lAltKey  | LK_RELEASED);
        if (e & USBK_MOD_LT_META  ) lk_write(lMetaKey | LK_RELEASED);
        if (e & USBK_MOD_RT_CTRL  ) lk_write(rCtrlKey | LK_RELEASED);
        if (e & USBK_MOD_RT_ALT   ) lk_write(rAltKey  | LK_RELEASED);
        if (e & USBK_MOD_RT_META  ) lk_write(rMetaKey | LK_RELEASED);
      } else if (e & USBK_EVENT_KEY_DOWN) {
        packet = e;
        if (packet == USBK_CAPS_LOCK) {
          ledState ^= USBK_LED_CAPS_LOCK;
          usbk_write_leds(ledState);
          if (ledState & USBK_LED_CAPS_LOCK) {
            lk_write(LK_CAPS_LOCK | LK_PRESSED);
          } else {
            lk_write(LK_CAPS_LOCK | LK_RELEASED);
          }
        } else if (
          !(modState & USBK_MOD_ANY_SHIFT) &&
          (modState & USBK_MOD_ANY_CTRL) &&
          (modState & USBK_MOD_ANY_ALT) &&
          packet >= USBK_F1 && packet <= USBK_F12
        ) {
          lk_write_blu_bootstrap(packet - USBK_F1);
        } else if ((e = lk_key_from_usb(packet, fkeyMap))) {
          lk_write_macro(e | LK_PRESSED);
        }
      } else if (e & USBK_EVENT_KEY_UP) {
        packet = e;
        if (packet == USBK_CAPS_LOCK) {
          // nop
        } else if (
          !(modState & USBK_MOD_ANY_SHIFT) &&
          (modState & USBK_MOD_ANY_CTRL) &&
          (modState & USBK_MOD_ANY_ALT) &&
          packet >= USBK_F1 && packet <= USBK_F12
        ) {
          // nop
        } else if ((e = lk_key_from_usb(packet, fkeyMap))) {
          lk_write_macro(e | LK_RELEASED);
        }
      }
    }
    lk_flush();
  }
}

int free_ram() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}
