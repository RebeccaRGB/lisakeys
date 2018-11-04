#include <Arduino.h>
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

static unsigned char keyReportBuf[8];
static unsigned char ledState = 0;

void usbk_start_output() {
  int i;
  for (i = 0; i < 8; i++) {
    keyReportBuf[i] = 0;
  }
  Serial.begin(9600);
  delay(100);
}

void usbk_key_pressed(unsigned char key) {
  int i;
  if ((key & 0xF8) == 0xE0) {
    keyReportBuf[0] |= (1 << (key & 0x07));
    Serial.write(keyReportBuf, 8);
    return;
  }
  for (i = 2; i < 8; i++) {
    if (keyReportBuf[i] == key) {
      keyReportBuf[i] = 0;
    }
  }
  for (i = 2; i < 8; i++) {
    if (!keyReportBuf[i]) {
      keyReportBuf[i] = key;
      break;
    }
  }
  Serial.write(keyReportBuf, 8);
}

void usbk_key_released(unsigned char key) {
  int i;
  if ((key & 0xF8) == 0xE0) {
    keyReportBuf[0] &=~ (1 << (key & 0x07));
    Serial.write(keyReportBuf, 8);
    return;
  }
  for (i = 2; i < 8; i++) {
    if (keyReportBuf[i] == key) {
      keyReportBuf[i] = 0;
    }
  }
  Serial.write(keyReportBuf, 8);
}

unsigned char usbk_read_leds() {
  int led = Serial.read();
  while (led >= 0) {
    ledState = led;
    led = Serial.read();
  }
  return ledState;
}

#ifdef USBK_USE_HOST_SHIELD

class UsbkParser : public KeyboardReportParser {
  public:
    UsbkParser() {
      eventBufStart = 0;
      eventBufEnd = 0;
    }

    unsigned int GetNextEvent() {
      if (eventBufStart == eventBufEnd) return 0;
      unsigned int event = eventBuffer[eventBufStart];
      eventBufStart++;
      eventBufStart &= (USBK_BUFFER_SIZE - 1);
      return event;
    }

  protected:
    unsigned int eventBuffer[USBK_BUFFER_SIZE];
    unsigned char eventBufStart;
    unsigned char eventBufEnd;

    void PutNextEvent(unsigned int event) {
      eventBuffer[eventBufEnd] = event;
      eventBufEnd++;
      eventBufEnd &= (USBK_BUFFER_SIZE - 1);
      if (eventBufEnd == eventBufStart) {
        eventBufEnd--;
        eventBufEnd &= (USBK_BUFFER_SIZE - 1);
      }
    }

    uint8_t HandleLockingKeys(USBHID * hid, uint8_t key) {
      return 0;
    }
    void OnControlKeysChanged(uint8_t prev, uint8_t next) {
      uint8_t pressed = next &~ prev;
      uint8_t released = prev &~ next;
      PutNextEvent(USBK_EVENT_MOD_STATE | next);
      if (pressed) PutNextEvent(USBK_EVENT_MOD_DOWN | pressed);
      if (released) PutNextEvent(USBK_EVENT_MOD_UP | released);
    }
    void OnKeyDown(uint8_t mod, uint8_t key) {
      if (key > USBK_UNDEFINED) PutNextEvent(USBK_EVENT_KEY_DOWN | key);
    }
    void OnKeyUp(uint8_t mod, uint8_t key) {
      if (key > USBK_UNDEFINED) PutNextEvent(USBK_EVENT_KEY_UP | key);
    }
};

static USB Usb;
static USBHub Hub(&Usb);
static HIDBoot<USB_HID_PROTOCOL_KEYBOARD> Kbd(&Usb);
static UsbkParser Prs;

#endif

void usbk_start_input() {
#ifdef USBK_USE_HOST_SHIELD
  Usb.Init();
  delay(200);
  Kbd.SetReportParser(0, &Prs);
#endif
}

unsigned int usbk_read_keys() {
#ifdef USBK_USE_HOST_SHIELD
  Usb.Task();
  return Prs.GetNextEvent();
#else
  return 0;
#endif
}

void usbk_write_leds(unsigned char leds) {
#ifdef USBK_USE_HOST_SHIELD
  ledState = leds;
  Kbd.SetReport(0, 0, 2, 0, 1, &ledState);
  Usb.Task();
#endif
}
