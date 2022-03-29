#include "DisplayPanel.h"
#include <Arduino.h>

void updateTimeTask( void * parameter )
{
  while (true)
  {
  }
}

void brightnessTask( void * parameter )
{
  DisplayPanel *displayPanel = (DisplayPanel *) parameter;

  while (true)
  {
    float lux = displayPanel->getLightMeterValue();

    if(lux < 5) displayPanel->setBrightnessAll(1);
    else        displayPanel->setBrightnessAll(100);
    delay(1000);
  }
}

void DisplayPanel::begin()
{
  uint8_t all_on[]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  for(unsigned int i=0; i<displayRows; i++)
  {
    for(unsigned int j=0; j<displayColumns; j++)
    {
      setBrightness(i, j, 100);
      displays[i][j]->setSegments(all_on, 6, 0, LAYER_TIME);
    }
  }

  Wire.begin();
  lightMeter.begin();
  xTaskCreate(
    brightnessTask,     /* Task function. */
    "brightnessTask",   /* String with name of task. */
    10000,              /* Stack size in bytes. */
    this,               /* Parameter passed as input of the task */
    1,                  /* Priority of the task. */
    &autoBrightnessTask /* Task handle. */
  );
  vTaskSuspend(autoBrightnessTask);

  xTaskCreate(
    updateTimeTask,     /* Task function. */
    "updateTimeTask",   /* String with name of task. */
    10000,              /* Stack size in bytes. */
    this,               /* Parameter passed as input of the task */
    1,                  /* Priority of the task. */
    NULL                /* Task handle. */
  );
}

// Clear custom segments, time segments and segment masks.
void DisplayPanel::clear()
{
    uint8_t all_off[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for(unsigned int i=0; i<displayRows; i++)
    {
      for(unsigned int j=0; j<displayColumns; j++)
      {
        for(unsigned int s=0; s<displays[row][0]->getSegmentsMax(); s++)
        {
          displays[i][j]->setActiveLayer(s, LAYER_TIME);
        }
        displays[i][j]->setSegments(all_off, 6, 0, LAYER_TIME);
        displays[i][j]->setSegments(all_off, 6, 0, LAYER_OVERWRITE);
      }
    }
}

void DisplayPanel::write()
{
  for(unsigned int i=0; i<displayRows; i++)
  {
    for(unsigned int j=0; j<displayColumns; j++)
    {
      displays[i][j]->write();
    }
  }
}

// 9 displays * 6 segments each = 54 segments
// Write segments across all 9 displays, starting at segment 0, top left
// ending at segment 53 bottom right
void DisplayPanel::overwriteSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
  // Boundary checks
  if(pos > segmentsMax-1)
    return;
  if((pos + length) > segmentsMax)
    return;

  // Handle segments in blocks (segments for each display).
  for(int segmentIterator = 0; segmentIterator < length; segmentIterator++)
  {
    // Which display is at "pos"? Example:
    // pos 0-5=display 0, pos 6-11=display1, pos 12-17=display2, ...
    uint8_t numDisplay = (pos+segmentIterator) / displays[0][0]->getSegmentsMax();

    // Determine row and column from previously determined display number
    uint8_t row =         numDisplay / displayRows;
    uint8_t column =      numDisplay % displayColumns;

    // Segment position within corresponding display
    // Example: pos=10: display 1, position 4 (0-indexed)
    uint8_t displayPos = (pos+segmentIterator) % displays[0][0]->getSegmentsMax();

    // How many bytes need to be written to this display?
    // Do all segments need to be updated or just a few?
    uint8_t lengthThisDisplay = displays[0][0]->getSegmentsMax() - displayPos;
    if((length - segmentIterator) < lengthThisDisplay)
      lengthThisDisplay = (length - segmentIterator);

    // Set segment masks / mark segments as custom segments, to avoid overwriting them
    // with time data. To show time segments again, segment mask needs to be cleared.
    for(int s=displayPos; s<displayPos+lengthThisDisplay; s++)
    {
      displays[row][column]->setActiveLayer(s, LAYER_OVERWRITE);
    }
    displays[row][column]->setSegments(segments+segmentIterator, lengthThisDisplay, displayPos, LAYER_OVERWRITE);

    // Skip over to next display.
    segmentIterator += lengthThisDisplay-1;
  }
}

void DisplayPanel::resetSegments(uint8_t length, uint8_t pos)
{
  // Boundary checks
  if(pos > segmentsMax-1)
    return;
  if((pos + length) > segmentsMax)
    return;

  // Handle segments in blocks (segments for each display).
  for(int segmentIterator = 0; segmentIterator < length; segmentIterator++)
  {
    // Which display is at "pos"? Example:
    // pos 0-5=display 0, pos 6-11=display1, pos 12-17=display2, ...
    uint8_t numDisplay = (pos+segmentIterator) / displays[0][0]->getSegmentsMax();

    // Determine row and column from previously determined display number
    uint8_t row =         numDisplay / displayRows;
    uint8_t column =      numDisplay % displayColumns;

    // Segment position within corresponding display
    // Example: pos=10: display 1, position 4 (0-indexed)
    uint8_t displayPos = (pos+segmentIterator) % displays[0][0]->getSegmentsMax();

    // How many bytes need to be written to this display?
    // Do all segments need to be updated or just a few?
    uint8_t lengthThisDisplay = displays[0][0]->getSegmentsMax() - displayPos;

    // It might be that not all bytes until the end of the display need to be written.
    // For example at the end of all bytes to be written. If needed, shorten length.
    if((length - segmentIterator) < lengthThisDisplay)
      lengthThisDisplay = (length - segmentIterator);

    // Set segment masks / mark segments as custom segments, to avoid overwriting them
    // with time data. To show time segments again, segment mask needs to be cleared.
    for(int s=displayPos; s<displayPos+lengthThisDisplay; s++)
    {
      displays[row][column]->setActiveLayer(s, LAYER_TIME);
    }

    // Skip over to next display.
    segmentIterator += lengthThisDisplay-1;
  }
}

void DisplayPanel::setBrightness(unsigned char row, unsigned char column, unsigned char brightness)
{
  if(row > displayRows) row = 0;
  if(column > displayColumns) column = 0;
  displays[row][column]->setBrightness(brightness);
}

void DisplayPanel::setBrightnessAll(unsigned char brightness)
{
  for(unsigned int i=0; i<displayRows; i++)
  {
    for(unsigned int j=0; j<displayColumns; j++)
    {
      setBrightness(i, j, brightness);
    }
  }
}

void DisplayPanel::setAutoBrightness(bool autoBrightness)
{
  if(autoBrightness)
  {
    vTaskResume(autoBrightnessTask);
  }
  else
  {
    vTaskSuspend(autoBrightnessTask);
  }
}

float DisplayPanel::getLightMeterValue()
{
  // value in lux
  return lightMeter.readLightLevel();
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

  displays[row][0]->setSegments(disp1, 4, 0, LAYER_TIME);
  displays[row][1]->setSegments(disp2, 4, 0, LAYER_TIME);
  displays[row][2]->setSegments(all_off, 6, 0, LAYER_TIME);
}

void DisplayPanel::showSoundError(int error_code)
{
  const uint8_t disp1[] = {
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,   // E
    SEG_E | SEG_G,                           // r
    SEG_E | SEG_G,                           // r
    0x00
  };
  const uint8_t disp2[] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,  // S
    SEG_C | SEG_E | SEG_G,                  // n
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,  // d
    0x00
  };
  const uint8_t disp3[] = {
    displays[row][0]->encodeDigit((error_code / 1000) % 10 ),
    displays[row][0]->encodeDigit((error_code / 100) % 10 ),
    displays[row][0]->encodeDigit((error_code / 10) % 10 ),
    displays[row][0]->encodeDigit( error_code % 10 )
  };

  displays[row][0]->setSegments(disp1, 4, 0, LAYER_TIME);
  displays[row][1]->setSegments(disp2, 4, 0, LAYER_TIME);
  displays[row][2]->setSegments(disp3, 4, 0, LAYER_TIME);
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

  displays[row][0]->setSegments(digits, 2, 2, LAYER_TIME);
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

  displays[row][0]->setSegments(digits, 2, 0, LAYER_TIME);
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

  displays[row][1]->setSegments(digits, 4, 0, LAYER_TIME);
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

  displays[row][2]->setSegments(digits, 5, 0, LAYER_TIME);
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