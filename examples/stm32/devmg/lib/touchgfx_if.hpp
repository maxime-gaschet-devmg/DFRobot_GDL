//******************************************************************************
//* Project :  ceva_oled_imvac_touchgfx
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

#ifndef TOUCHGFXAPI_HPP_
#define TOUCHGFXAPI_HPP_

#include <touchgfx/hal/HAL.hpp>

#include "gpio.h" //$TEMP

#include "drvKpad.h"
#include "cmpOled_nhd-2.7-12864wd.h"

//******************************************************************************
// Name: touchgfx_flushFrameBuffer()
// Params: void
// Return: void
// Brief: called whenever frame buffer needs to be transfered to display.
//******************************************************************************
extern "C" void touchgfx_flushFrameBuffer(int16_t x, int16_t y, int16_t w, int16_t h,
                          uint8_t *frameBuffer, uint16_t lineSize,
                          int bitDepth)
{
  HAL_GPIO_WritePin(TEST_PIN_GPIO_Port, TEST_PIN_Pin, GPIO_PIN_SET);

  // x must be multiple of x because x pixels per byte.
  while(x % (8 / bitDepth)) {
    x--;
    w++;
  }
  // Width must be multiple of 2, because we write 2 bytes per 2 bytes.
  while(w % (8 / bitDepth)) {
    w++;
  }
  // Check boundaries.
  if(x > 127)
    x = 127;
  if(y > 63)
    y = 63;
  if(x + w > 128)
    w = 128 - x;
  if(y + h > 64)
    h = 64 - y;
  cmpOled_setDisplayWindow(x, y, w, h);
  frameBuffer += y * lineSize + x / (8 / bitDepth); // Place pointer to beginning of rectangle.
  while(h > 0)
  {
    if(1 == bitDepth) {
      cmpOled_writeData_1bpp(frameBuffer, w); // BW.
    }
    else {
      cmpOled_writeData_4bpp(frameBuffer, w); // Gray4.
    }
    frameBuffer += lineSize;
    h--;
  }
  HAL_GPIO_WritePin(TEST_PIN_GPIO_Port, TEST_PIN_Pin, GPIO_PIN_RESET);
}

// Child class of ButtonController: required to detect hardware buttons.
class HwButtonController : public touchgfx::ButtonController
{
public:
  virtual void init() override {
  }

  // Function called at each tick to check if a key has been striked.
  virtual bool sample(uint8_t& key) override
  {
    keyPad_t kpadKey;
    bool bRet = false;
    if(drvKpad_getKeyPressed(&kpadKey))
    {
      // Must convert keypad's key to system key.
      switch(kpadKey)
      {
        case key_2:    key = '8'; bRet = true; break; // Up.
        case key_4:    key = '4'; bRet = true; break; // Left.
        case key_5:    key = '5'; bRet = true; break; // Ok.
        case key_6:    key = '6'; bRet = true; break; // Right.
        case key_8:    key = '2'; bRet = true; break; // Down.
        case key_9:    key = '3'; bRet = true; break; // Cancel.
        case key_B:    key = 'b'; bRet = true; break; // Battery (simulation).
        case key_A:    key = 'p'; bRet = true; break; // Pushbutton (simulation).
        case key_none: key = 'r'; bRet = true; break; // Released.
        default: break;                            // Other keys are unconsidered.
      }
    }
    return bRet;
  }

private:
};

#endif

// EOF *************************************************************************
