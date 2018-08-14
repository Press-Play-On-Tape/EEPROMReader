#define USE_AVR_EEPROM

#include <Arduboy2.h>
#include "src/Font4x6.h"

#ifdef USE_AVR_EEPROM
  #include <avr/eeprom.h>
#else
  #include <EEPROM.h>
#endif

Arduboy2Base arduboy;
Sprites sprite;
Font4x6 font4x6 = Font4x6(0);

enum class EditMode : uint8_t {

  None,
  Edit

};

char text1[8];
char text2[8];
bool flash = false;
uint8_t origValue;
EditMode mode = EditMode::None;

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
  arduboy.setFrameRate(15);

}


//------------------------------------------------------------------------------
//  Loop away!
//------------------------------------------------------------------------------

#define SPACING_X 13
#define SPACING_Y 8

void loop() {
  
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();

  arduboy.clear();

  arduboy.drawFastHLine(0, 10, WIDTH);
  arduboy.drawFastHLine(0, 32, 65);
  arduboy.drawFastHLine(0, 45, 65);
  arduboy.drawLine(65, 32, 69, 16, WHITE);
  arduboy.drawLine(65, 45, 69, 61, WHITE);

  font4x6.setCursor(2, 0);
  font4x6.print(F("Location "));
  font4x6.print(eepromLocation);

  for (int16_t x = eepromLocation - 12, y = 0; x < eepromLocation + 13; x++, y++) {

    uint16_t location = (x < 0 ? x + 1024 : (x > 1023 ? x - 1024 : x));

    #ifdef USE_AVR_EEPROM
      uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(location);
      uint8_t val = eeprom_read_byte(eepromAddress);
    #else
      uint8_t val = EEPROM.read(location);
    #endif

    switch (y / 5) {

      case 0 ... 1:
        font4x6.setCursor(((y % 5) * SPACING_X) + 2, ((y / 5) * SPACING_Y) + 14);
        break;

      case 2:

        if ((y % 5) == 2) {

          if (mode == EditMode::None || flash) {

            arduboy.fillRect(((y % 5) * SPACING_X), ((y / 5) * SPACING_Y) + 19, 13, 8, WHITE);
            font4x6.setTextColor(BLACK);

          }

        }
        else {

          font4x6.setTextColor(WHITE);

        }

        font4x6.setCursor(((y % 5) * SPACING_X) + 2, ((y / 5) * SPACING_Y) + 19);
        break;

      case 3 ... 4:
        font4x6.setCursor(((y % 5) * SPACING_X) + 2, ((y / 5) * SPACING_Y) + 24);
        break;

    }

    printDec(val);
    
  }


  printChar(75, 15, static_cast<uint16_t>(eepromLocation - 2));
  printBinary(84, 15, static_cast<uint16_t>(eepromLocation - 2));

  printChar(75, 25, static_cast<uint16_t>(eepromLocation - 1));
  printBinary(84, 25, static_cast<uint16_t>(eepromLocation - 1));

  arduboy.fillRect(74, 35, 6, 8, WHITE);
  arduboy.fillRect(83, 35, 21, 8, WHITE);
  arduboy.fillRect(107, 35, 21, 8, WHITE);
  font4x6.setTextColor(BLACK);
  printChar(75, 35, static_cast<uint16_t>(eepromLocation));
  printBinary(84, 35, static_cast<uint16_t>(eepromLocation));
  font4x6.setTextColor(WHITE);

  printChar(75, 45, static_cast<uint16_t>(eepromLocation + 1));
  printBinary(84, 45, static_cast<uint16_t>(eepromLocation + 1));
  printChar(75, 55, static_cast<uint16_t>(eepromLocation + 2));
  printBinary(84, 55, static_cast<uint16_t>(eepromLocation + 2));


  switch (mode) {

    case EditMode::None:
      if (arduboy.justPressed(LEFT_BUTTON))   { --eepromLocation; if (eepromLocation < 0) eepromLocation = 1023; }
      if (arduboy.justPressed(RIGHT_BUTTON))  { ++eepromLocation; if (eepromLocation > 1023) eepromLocation = 0; }
      if (arduboy.justPressed(UP_BUTTON))     { eepromLocation = eepromLocation - 5; if (eepromLocation < 0) eepromLocation = eepromLocation + 1024; }
      if (arduboy.justPressed(DOWN_BUTTON))   { eepromLocation = eepromLocation + 5; if (eepromLocation > 1023) eepromLocation = eepromLocation - 1024; }

      if (arduboy.justPressed(A_BUTTON))      { 
        mode = EditMode::Edit; 
        #ifdef USE_AVR_EEPROM
          uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(eepromLocation);
          origValue = eeprom_read_byte(static_cast<uint8_t *>(eepromAddress)); 
        #else
          origValue = EEPROM.read(eepromLocation); 
        #endif
      }

      break;

    case EditMode::Edit:

      font4x6.setCursor(93, 0);
      font4x6.print(F("Orig "));
      printDec(origValue);

      if (arduboy.pressed(UP_BUTTON))         incValue();
      if (arduboy.pressed(DOWN_BUTTON))       decValue();
      if (arduboy.justPressed(A_BUTTON))      mode = EditMode::None;
      break;

  }

  if (arduboy.everyXFrames(32)) { flash = !flash; }
  arduboy.display();

}

void printDec(uint8_t data) {

  if (data < 16) { font4x6.print(F("0")); }
  font4x6.print(data, HEX);

}


void printChar(uint8_t x, uint8_t y, uint16_t location) {

  #ifdef USE_AVR_EEPROM
    uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(location);
    uint8_t data = eeprom_read_byte(eepromAddress);
  #else
    uint8_t data = EEPROM.read(location);
  #endif

  font4x6.setCursor(x, y); 
  font4x6.print((char)data);

}

void printBinary(uint8_t x, uint8_t y, uint16_t location) {

  #ifdef USE_AVR_EEPROM
    uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(location);
    uint8_t data = eeprom_read_byte(eepromAddress);
  #else
    uint8_t data = EEPROM.read(location);
  #endif

  sprintf(text1, "%c%c%c%c",
    (data & 0x80) ? '1' : '0',
    (data & 0x40) ? '1' : '0',
    (data & 0x20) ? '1' : '0',
    (data & 0x10) ? '1' : '0'
  );

  sprintf(text2, "%c%c%c%c",
    (data & 0x08) ? '1' : '0',
    (data & 0x04) ? '1' : '0',
    (data & 0x02) ? '1' : '0',
    (data & 0x01) ? '1' : '0'
  );

  font4x6.setCursor(x, y); 
  font4x6.print(text1);
  font4x6.setCursor(x + 24, y); 
  font4x6.print(text2);

}

void incValue() {

  #ifdef USE_AVR_EEPROM
    uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(eepromLocation);
    uint8_t data = eeprom_read_byte(eepromAddress);
    eeprom_update_byte(eepromAddress, data + 1);
  #else
    uint8_t data = EEPROM.read(eepromLocation);
    EEPROM.update(eepromLocation, data + 1);
  #endif

}

void decValue() {

  #ifdef USE_AVR_EEPROM
    uint8_t * eepromAddress = reinterpret_cast<uint8_t *>(eepromLocation);
    uint8_t data = eeprom_read_byte(eepromAddress);
    eeprom_update_byte(eepromAddress, data - 1);
  #else
    uint8_t data = EEPROM.read(eepromLocation);
    EEPROM.update(eepromLocation, data - 1);
  #endif
  
}