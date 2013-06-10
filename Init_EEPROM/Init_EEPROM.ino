#include <EEPROM.h>

int address = 0;
byte whichBot = 5;

// the setup routine runs once when you press reset:
void setup() {
  EEPROM.write(address,whichBot);
}

// the loop routine runs over and over again forever:
void loop() {
  //do nothing
}
