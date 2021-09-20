#include "DisplayPanel.h"
#include <Arduino.h>

// const uint8_t SEG_DONE[] = {
// 	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
// 	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
// 	SEG_C | SEG_E | SEG_G,                           // n
// 	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
// };

void DisplayPanel::begin()
{
  uint8_t all_on[]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  for(unsigned int i=0; i<displayRows; i++)
  {
    for(unsigned int j=0; j<displayColumns; j++)
    {
      setBrightness(i, j, 7);
      displays[i][j]->setSegments(all_on, 6);
    }
  }
  
	// // Run through all the dots
	// for(int k=0; k <= 4; k++) {
	// 	displays[1][2]->showNumberDecEx(0, (0x80 >> k), true);
	// 	delay(TEST_DELAY);
	// }

  //displays[1][2]->setBrightness(k);
}

void DisplayPanel::clear()
{
    uint8_t all_off[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for(unsigned int i=0; i<displayRows; i++)
    {
      for(unsigned int j=0; j<displayColumns; j++)
      {
        displays[i][j]->setSegments(all_off, 6);
      }
    }
}

void DisplayPanel::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
  if(pos > segmentsMax-1)
    pos = segmentsMax-1;

  if((pos + length) > segmentsMax)
    length = segmentsMax - pos;

  for(int i = 0; i < length; i++)
  {
    uint8_t numDisplay = (pos+i) / displays[0][0]->getSegmentsMax();
    uint8_t displayPos = (pos+i) % displays[0][0]->getSegmentsMax();
    uint8_t row =         numDisplay / displayRows;
    uint8_t column =      numDisplay % displayColumns;

    uint8_t lengthThisDisplay = displays[0][0]->getSegmentsMax() - displayPos;
    if((length - i) < lengthThisDisplay) lengthThisDisplay = (length - i);

    char mystr[100];
    sprintf(mystr, "displays[%d][%d]->setSegments(segments+%d, %d, %d);", row, column, i, lengthThisDisplay, displayPos);
    // Serial.println(mystr);
    displays[row][column]->setSegments(segments+i, lengthThisDisplay, displayPos);

    i += lengthThisDisplay-1;
  }
}

void DisplayPanel::setBrightness(unsigned char row, unsigned char column, unsigned char brightness, bool on)
{
  if(row > displayRows) row = 0;
  if(column > displayColumns) column = 0;
  displays[row][column]->setBrightness(brightness, on);
}

void DisplayPanel::showRTCError()
{
  const uint8_t disp1[] = {
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,   // E
    SEG_E | SEG_G,                           // r
    SEG_E | SEG_G,                           // r
    0x00
  };
  const uint8_t disp2[] = {
    SEG_E | SEG_G,                          // r
    SEG_D | SEG_E | SEG_F | SEG_G,          // t
    SEG_D | SEG_E | SEG_G,                  // c
    0x00
  };
  const uint8_t all_off[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  displays[row][0]->setSegments(disp1);
  displays[row][1]->setSegments(disp2);
  displays[row][2]->setSegments(all_off, 6);
}

void DisplayPanel::showCommandInterfaceError()
{
  const uint8_t disp1[] = {
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,   // E
    SEG_E | SEG_G,                           // r
    SEG_E | SEG_G,                           // r
    0x00
  };
  const uint8_t disp2[] = {
    SEG_E | SEG_G,                          // r
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,  // d
    SEG_C,                                  // i
    SEG_C | SEG_D | SEG_E | SEG_G           // o
  };
  const uint8_t all_off[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

  displays[row][0]->setSegments(disp1);
  displays[row][1]->setSegments(disp2);
  displays[row][2]->setSegments(all_off, 5);
}

void DisplayPanel::setRow(unsigned int row)
{
  if(row > displayRows) row = 0;
  this->row = row;
}

void DisplayPanel::setMonth(unsigned char month)
{
  uint8_t digits[2];
  if(month <= 12 && month >= 1)
  {
    digits[0] = displays[row][0]->encodeDigit((month / 10) % 10 );
    digits[1] = displays[row][0]->encodeDigit( month % 10 );
  }
  else
  {
    // show "--"
    digits[0] = 0x00 | SEG_G;
    digits[1] = 0x00 | SEG_G;
  }

  // Set dot
  digits[1] |= SEG_DP;

  displays[row][0]->setSegments(digits, 2, 2);
}

void DisplayPanel::setDay(unsigned char day)
{
  uint8_t digits[2];
  if(day <= 31 && day >= 1)
  {
    digits[0] = displays[row][0]->encodeDigit((day / 10) % 10 );
    digits[1] = displays[row][0]->encodeDigit( day % 10 );
  }
  else
  {
    // show "--"
    digits[0] = 0x00 | SEG_G;
    digits[1] = 0x00 | SEG_G;
  }

  // Set dot
  digits[1] |= SEG_DP;

  displays[row][0]->setSegments(digits, 2, 0);
}

void DisplayPanel::setYear(unsigned int year)
{
  uint8_t digits[4];
  if(year <= 9999)
  {
    for(int i = 3; i>=0; i--)
    {
      digits[i] = displays[row][1]->encodeDigit(year % 10);
      year /= 10;
    }
  }
  else
  {
    // show "----"
    digits[0] = 0x00 | SEG_G;
    digits[1] = 0x00 | SEG_G;
    digits[2] = 0x00 | SEG_G;
    digits[3] = 0x00 | SEG_G;
  }

  // Turn off dots
  digits[0] &= ~SEG_DP;
  digits[1] &= ~SEG_DP;
  digits[2] &= ~SEG_DP;
  digits[3] &= ~SEG_DP;

  displays[row][1]->setSegments(digits);
}

// Needs to be set together due to problem in TM1637 lib 
// (cannot set am/pm without overwriting minute).
void DisplayPanel::setHourAndMinute(unsigned char hour, unsigned char minute)
{
  uint8_t digits[5];
  if(hour <= 24)
  {
    if(hour == 24) hour = 0;
    
    digits[0] = displays[row][2]->encodeDigit((hour / 10) % 10 );
    digits[1] = displays[row][2]->encodeDigit( hour % 10 );
    
    // Set AM/PM
    if(hour >= 0 && hour <= 11) digits[4] = 0x01;
    else                        digits[4] = 0x10;
  }
  else
  {
    // show "--"
    digits[0] = 0x00 | SEG_G;
    digits[1] = 0x00 | SEG_G;
    // turn off AM/PM
    digits[4] = 0x00;
  }

  if(minute <= 60)
  {
    if(minute == 60) minute = 0;

    digits[2] = displays[row][2]->encodeDigit((minute / 10) % 10 );
    digits[3] = displays[row][2]->encodeDigit( minute % 10);
  }
  else
  {
    // show "--"
    digits[2] = 0x00 | SEG_G;
    digits[3] = 0x00 | SEG_G;
  }
  
  // Set colon
  digits[0] &= ~SEG_DP;
  digits[1] |= SEG_DP;
  digits[2] &= ~SEG_DP;
  digits[3] &= ~SEG_DP;

  displays[row][2]->setSegments(digits, 5, 0);
}

uint8_t DisplayPanel::getSegmentsMax()
{
  return segmentsMax;
}

uint8_t DisplayPanel::getRows()
{
  return displayRows;
}

uint8_t DisplayPanel::getColumns()
{
  return displayColumns;
}

// TODO: Save displayPanel data internally, check if content changed, only update displayPanel if content changed.
// TODO: For setday, setmonth etc, use setSegments instead of displays directly