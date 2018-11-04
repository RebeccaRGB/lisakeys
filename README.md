# LisaKeys
Apple Lisa Keyboard Tester/Translator/Emulator

This Arduino sketch can function in four different modes:

* Tester Mode: Capture keystrokes from a Lisa keyboard and print them over serial.
* Forward Translator Mode: Use a Lisa keyboard as a USB keyboard on a modern device.
* Emulator Mode: Read instructions over serial and send keystrokes to a Lisa.
* Reverse Translator Mode: Use a modern USB keyboard as a keyboard on a Lisa.

There are also gerber files for a shield to make connections and configuration easier.

## Connections

### Lisa Keyboard Connections
* Lisa keyboard VCC, tip of TRS connector: connect to Arduino +5V.
* Lisa keyboard DATA, ring of TRS connector: connect to Arduino digital pin 6.
* Lisa keyboard GND, sleeve of TRS connector: connect to Arduino GND.

### Arduino Connections
* Digital pins 0-5: used to select settings.
* Digital pin 6: connect to Lisa keyboard DATA line.
* Digital pins 7-13: reserved for USB Host Shield.
* Analog pins 0-5: used for a keypad.

### "DIP Switch" Settings
Digital pins 0-5 are used to select the mode of operation and settings for that mode. These are present on the shield as switches or jumpers labeled A-F. To leave the "DIP switch" in the OPEN position, leave the pin disconnected or set the switch to the UP position. To leave the "DIP switch" in the CLOSED position, connect the pin to GND or set the switch to the DOWN position.

* Digital pin 5 / switch A:
* * OPEN: `LK>` - Lisa Keyboard In. Tester Mode or Forward Translator Mode. Connect Lisa keyboard to Arduino and Arduino to modern device.
* * CLOSED: `>LH` - To Lisa Host. Emulator Mode or Reverse Translator Mode. Connect modern device to Arduino and Arduino to Lisa.
* Digital pin 4 / switch B:
* * OPEN: `SER` - Serial. Tester Mode or Emulator Mode. Arduino communicates with a modern device over USB serial.
* * CLOSED: `USB`. Translator Mode. Arduino acts as a USB keyboard device or host, depending on switch A.
* Digital pin 3 / switch C:
* * In Forward Translator Mode:
* * * OPEN: `PC` - PC Modifiers. Left Option = Ctrl, Apple = Alt, Enter = Meta (or Right Alt), Right Option = Fn.
* * * CLOSED: `MAC` - Mac Modifiers. Left Option = Alt, Apple = Meta, Enter = Ctrl (or Right Meta), Right Option = Fn.
* * In Emulator or Reverse Translator Mode:
* * * OPEN: `US/UK`. Selects US or UK keyboard ID.
* * * CLOSED: `DE/FR`. Selects German or French keyboard ID.
* Digital pin 2 / switch D:
* * In Forward Translator Mode:
* * * OPEN: `LFT` - Left Modifiers. Enter = Meta or Ctrl.
* * * CLOSED: `MIR` - Mirrored Modifiers. Enter = Right Alt or Right Meta.
* * In Emulator or Reverse Translator Mode:
* * * OPEN: `US/DE`. Selects US or German keyboard ID.
* * * CLOSED: `UK/FR`. Selects UK or French keyboard ID.
* Digital pin 1 / switch E:
* * In Reverse Translator Mode:
* * * OPEN: `PC` - PC Modifiers. Ctrl/Meta = Option, Alt = Apple, Right Alt = Enter.
* * * CLOSED: `MAC` - Mac Modifiers. Alt = Option, Ctrl/Meta = Apple, Right Ctrl/Meta = Enter.
* * In all other modes, must be OPEN.
* Digital pin 0 / switch F:
* * In Reverse Translator Mode:
* * * OPEN: `LFK` - Lisa FKeys. F1-F12 = Right Option + Shift + numeric keypad.
* * * CLOSED: `MFK` - Mac FKeys. F1-F12 = Apple + Shift + number row.
* * In all other modes, must be OPEN.
