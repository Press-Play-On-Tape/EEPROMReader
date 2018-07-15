#include <Arduboy2.h>
#include "src/Font4x6.h"
#include <EEPROM.h>

Arduboy2Base arduboy;
Sprites sprite;

Font4x6 font4x6 = Font4x6(0);
char text[24];

int16_t eepromLocation = EEPROM_STORAGE_SPACE_START;

//------------------------------------------------------------------------------
//  Setup 
//------------------------------------------------------------------------------

void setup() {

  arduboy.boot();
  arduboy.display();
  arduboy.flashlight();
  arduboy.systemButtons();
  arduboy.clear();
  arduboy.setFrameRate(60);

}

void loop() {
  
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();

  arduboy.clear();

  arduboy.drawFastHLine(0, 10, WIDTH);
  arduboy.drawFastHLine(0, 32, 71);
  arduboy.drawFastHLine(0, 45, 71);
  arduboy.drawLine(71, 32, 79, 16, WHITE);
  arduboy.drawLine(71, 45, 79, 61, WHITE);

  font4x6.setCursor(2, 0);
  font4x6.print("Location ");
  font4x6.print(eepromLocation);

  for (int16_t x = eepromLocation - 12, y = 0; x < eepromLocation + 13; x++, y++) {

    uint8_t val = EEPROM.read((x < 0 ? x + 1024 : (x > 1023 ? x - 1024 : x)));

    switch (y / 5) {

      case 0 ... 1:
        font4x6.setCursor(((y % 5) * 15) + 2, ((y / 5) * 8) + 14);
        break;

      case 2:

        if ((y % 5) == 2) {

          arduboy.fillRect(((y % 5) * 15), ((y / 5) * 8) + 19, 13, 8, WHITE);
          font4x6.setTextColor(BLACK);

        }
        else {

          font4x6.setTextColor(WHITE);

        }

        font4x6.setCursor(((y % 5) * 15) + 2, ((y / 5) * 8) + 19);
        break;

      case 3 ... 4:
        font4x6.setCursor(((y % 5) * 15) + 2, ((y / 5) * 8) + 24);
        break;

    }

    if (val < 16) { font4x6.print("0"); }
    font4x6.print(val, HEX);

  }

  font4x6.setCursor(83, 15);
  printBinary(static_cast<uint16_t>(eepromLocation - 2));
  font4x6.setCursor(83, 25);
  printBinary(static_cast<uint16_t>(eepromLocation - 1));

  arduboy.fillRect(81, 35, 22, 8, WHITE);
  arduboy.fillRect(106, 35, 22, 8, WHITE);
  font4x6.setTextColor(BLACK);
  font4x6.setCursor(83, 35); 
  printBinary(static_cast<uint16_t>(eepromLocation));
  font4x6.setTextColor(WHITE);

  font4x6.setCursor(83, 45);
  printBinary(static_cast<uint16_t>(eepromLocation + 1));
  font4x6.setCursor(83, 55);
  printBinary(static_cast<uint16_t>(eepromLocation + 2));


  if (arduboy.justPressed(LEFT_BUTTON))   --eepromLocation; if (eepromLocation < 0) eepromLocation = 1023;
  if (arduboy.justPressed(RIGHT_BUTTON))  ++eepromLocation; if (eepromLocation > 1023) eepromLocation = 0;
  if (arduboy.justPressed(UP_BUTTON))     eepromLocation = eepromLocation - 5; if (eepromLocation < 0) eepromLocation = eepromLocation + 1024;
  if (arduboy.justPressed(DOWN_BUTTON))   eepromLocation = eepromLocation + 5; if (eepromLocation > 1023) eepromLocation = eepromLocation - 1024;

  arduboy.display();

}

void printBinary(uint16_t location) {

  uint8_t data = EEPROM.read(location);

  sprintf(text, "%c%c%c%c %c%c%c%c",
    (data & 0x80) ? '1' : '0',
    (data & 0x40) ? '1' : '0',
    (data & 0x20) ? '1' : '0',
    (data & 0x10) ? '1' : '0',
    (data & 0x08) ? '1' : '0',
    (data & 0x04) ? '1' : '0',
    (data & 0x02) ? '1' : '0',
    (data & 0x01) ? '1' : '0'
  );

  font4x6.print(text);

}