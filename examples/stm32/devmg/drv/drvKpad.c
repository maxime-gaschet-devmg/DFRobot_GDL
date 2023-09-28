//******************************************************************************
//* Project :
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

//******************************************************************************
// Driver / Service: driver for matrix row-column keypad. Specific to stm32.
// Use: this driver detects only one key pressed and must be polled.
// - pins connected to row must be configured in cubeMx as outputs push-pull high level.
// - pins connected to colums must be configured in cubeMx as inputs with pull-down.
//******************************************************************************

// INCLUDES ********************************************************************

#include <drvKpad.h>
#include <drvTime.h>
#include <stdint.h>

#include "gpio.h"


// DEFINES *********************************************************************

#define PERIOD_BETWEEN_ROWS_MS (10)

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

// Pins connection to row / column.
/*  Keypad arrangement.
 * D7 row0 -> 1  2  3  A
 * D6 row1 -> 4  5  6  B
 * D5 row2 -> 7  8  9  C
 * D4 row3 -> *  0  #  D
 *            ^  ^  ^  ^
 *            |  |  |  |
 *         col0  1  2  3
 *           D3 D2 D1 D0
 */
static GPIO_TypeDef *aGpioRow[4] = {D7_GPIO_Port, D6_GPIO_Port, D5_GPIO_Port, D4_GPIO_Port};
static uint16_t       aPinRow[4] = {D7_Pin,       D6_Pin,       D5_Pin,       D4_Pin      };
static GPIO_TypeDef *aGpioCol[4] = {D3_GPIO_Port, D2_GPIO_Port, D1_GPIO_Port, D0_GPIO_Port};
static uint16_t       aPinCol[4] = {D3_Pin,       D2_Pin,       D1_Pin,       D0_Pin      };
static keyPad_t aKeyColRow[4][4] = {{key_1,    key_2,     key_3,     key_A},
                                    {key_4,    key_5,     key_6,     key_B},
                                    {key_7,    key_8,     key_9,     key_C},
                                    {key_star, key_0,     key_pound, key_D}};
static GPIO_InitTypeDef initGpio = {0};
static keyPad_t keyPressed = key_none;
static keyPad_t keyLastRead = key_none;
static tempo_t tempo;
static uint8_t rowNum;

// FUNCTIONS *******************************************************************

// Private.
void enableRow(uint8_t row, bool bEnable);
void scanColumns(uint8_t row, keyPad_t *key);

//******************************************************************************
// Name: drvKpad_init()
// Params: void
// Return: void
// Brief: init driver.
//******************************************************************************
void drvKpad_init(void)
{
  // Start tempo.
  drvTime_startTempo_ms(&tempo, PERIOD_BETWEEN_ROWS_MS);

  // We will begin at row0.
  rowNum = 0;
  HAL_GPIO_WritePin(aGpioRow[0], aPinRow[0], GPIO_PIN_SET);

}

//******************************************************************************
// Name: drvKpad_task()
// Params: void
// Return: void
// Brief: driver's task.
//******************************************************************************
void drvKpad_task()
{
  // $TODO: implement a debouncer.
  if(drvTime_isElapsed(&tempo))
  {
    drvTime_restartTempo(&tempo);

    static keyPad_t newKeyPressed;
    if(0 == rowNum) {
      newKeyPressed = key_none;
    }
    scanColumns(rowNum, &newKeyPressed);
    if(3 == rowNum) {
      keyPressed = newKeyPressed;
    }

    // Proceed to new row.
    enableRow(rowNum++, false);
    rowNum %= 4;
    enableRow(rowNum, true);
  }
}

//******************************************************************************
// Name: drvKpad_getKeyPressed()
// Params: pointer to the key pressed, valid only if return is true.
// Return: true if a key is pressed.
// Brief: return true if a key is pressed, the pointer in input is updated.
//******************************************************************************
bool drvKpad_getKeyPressed(keyPad_t *key)
{
  bool bRet = false;
  if(keyLastRead != keyPressed)
  {
//    if(keyPressed != key_none)  // Return true, even when a key is just released.
    {
      *key = keyPressed;
      bRet = true;
    }
    keyLastRead = keyPressed; // Save.
  }
  return bRet;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: enableRow()
// Params: row number to enable/disable, boolean to enable/disable.
// Return: void
// Brief: enable a row of the matrix keypad.
//******************************************************************************
void enableRow(uint8_t row, bool bEnable)
{
/*
 *  When enabled, the pin connected to the row is output pushpull with high level.
 *  When disabled, the pin is input with internal pull-down.
 *  This is one like this to avoid short circuit if 2 buttons are pushed at the
 *  same time.
 */
  initGpio.Pin = aPinRow[row];
  if(bEnable) {
    initGpio.Pull = GPIO_NOPULL;
    initGpio.Mode = GPIO_MODE_OUTPUT_PP;
  }
  else {
    initGpio.Pull = GPIO_PULLDOWN; // Pulldown recommended when disabled (input).
    initGpio.Mode = GPIO_MODE_INPUT;
  }
  HAL_GPIO_Init(aGpioRow[row], &initGpio);
}

//******************************************************************************
// Name: enableRow()
// Params: row number to enable/disable, boolean to enable/disable.
// Return: void
// Brief: enable a row of the matrix keypad.
//******************************************************************************
void scanColumns(uint8_t row, keyPad_t *key)
{
  static uint8_t col;
  col = 0;
  while(col < 4)
  {
    if(HAL_GPIO_ReadPin(aGpioCol[col], aPinCol[col]))
    {
      *key = aKeyColRow[row][col];
      break;
    }
    col++;
  }
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************



// EOF *************************************************************************
