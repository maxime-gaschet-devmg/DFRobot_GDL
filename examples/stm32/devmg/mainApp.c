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
//* Hardware Information:
//* - MCU       : x (xkB Flash / xkB Ram / xB Ee / xMHz)
//* - IDE       : x
//* - COMPILER  : x
//* - OPTIM     : x
//* - MEM USAGE : x% / x%
//******************************************************************************

// Notes: on NUCLEO-L476RG
//  - SB13 / SB14 / SB15 must be unsoldered.
//  - SB62 / SB32 must be soldered.

// INCLUDES ********************************************************************

#include <drvKpad.h>
#include <drvTime.h>
#include <lib/touchgfx_if.h>
#include "mainApp.h"

#include "spi.h"


// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

tempo_t tempo;
//uint8_t instLedV; // Led debug.

static const uint8_t /*PROGMEM*/ DFRobot_ILI9341_initCmd[]= {
//flag cmd  If the highest bit is 1, it means that the last 2 bits are delay, the lower 7 bits indicate the number of parameters
0x01,0xCF,3,0x00,0xC1,0X30,
0x01,0xED,4,0x64,0x03,0X12,0X81,
0x01,0xE8,3,0x85,0x00,0x78,
0x01,0xCB,5,0x39,0x2C,0x00,0x34,0x02,
0x01,0xF7,1,0x20,
0x01,0xEA,2,0x00,0x00,
0x01,0xC0,1,0x10,
0x01,0xC1,1,0x00,
0x01,0xC5,2,0x30,0x30,
0x01,0xC7,1,0xB7,
0x01,0x3A,1,0x55,
0x01,0x36,1,0x08,
0x01,0xB1,2,0x00,0x1A,
0x01,0xB6,2,0x0A ,0xA2,
0x01,0xF2,1,0x00,
0x01,0x26,1,0x01,
0x01,0xE0,15,0x0F,0x2A,0x28,0x08,0x0E,0x08,0x54,0xA9,0x43,0x0A,0x0F,0x00,0x00,0x00,0x00,
0x01,0XE1,15,0x00,0x15,0x17,0x07,0x11,0x06,0x2B,0x56,0x3C,0x05,0x10,0x0F,0x3F,0x3F,0x0F,
0x01,0x2B,4,0x00,0x00,0x01,0x3f,
0x01,0x2A,4,0x00,0x00,0x00,0xef,
0x01, 0x11,0x80,0,120,
0x01, 0x29, 0,
0x01, 0x36, 0, //add
0x01, 0x08, 0, //add
0x00,
};


// FUNCTIONS *******************************************************************

// Private.

// Callbacks.
//void callbackGreenLed(bool ledOn);
#define mCS_Enable()  HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET)
#define mCS_Disable() HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET)
#define mRes_enable()  HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, GPIO_PIN_RESET)
#define mRes_disable() HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, GPIO_PIN_SET)
#define mDC_Data()    HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_SET)
#define mDC_Cmd()     HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_RESET)

void sendCmd(uint8_t cmd)
{
  mCS_Enable();
  drvTime_wait_us(1);
  mDC_Cmd();
  drvTime_wait_us(1);
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
  drvTime_wait_us(1);
  mCS_Disable();
  drvTime_wait_us(1);
  mDC_Data();

  drvTime_wait_us(7);
}

void sendData(uint8_t *pDataBuf, uint16_t dataLen)
{
  mCS_Enable();
  drvTime_wait_us(1);
  HAL_SPI_Transmit(&hspi1, pDataBuf, dataLen, 10);
  drvTime_wait_us(1);
  mCS_Disable();

  drvTime_wait_us(10);
}


uint8_t *sendInitCmd(uint8_t *pBuf)
{
  uint8_t cmdLen = pBuf[0];
  uint8_t *pCmdBuf = &pBuf[1];
  uint8_t dataLen = pBuf[1 + cmdLen];
  uint8_t *pDataBuf = &pBuf[2 + cmdLen];
  if(cmdLen > 0)
  {
    sendCmd(*pCmdBuf);

    if(dataLen > 0)
    {
      if(dataLen == 0x80)
      {
        uint16_t delay = (pBuf[2 + cmdLen] << 8) + pBuf[3 + cmdLen];
        drvTime_wait_ms(delay);
        dataLen = 2;
      }
      else {
        sendData(pDataBuf, dataLen);
      }
    }
    return pBuf + 2 + cmdLen + dataLen;
  }
  return NULL;
}

void initTft()
{

  mCS_Enable(); // $TEMP
  mCS_Disable();

  mRes_enable();
  drvTime_wait_ms(100);
  mRes_disable();
  drvTime_wait_ms(100);

  uint8_t *pBuf = (uint8_t *)DFRobot_ILI9341_initCmd;
  while(pBuf != NULL) {
    pBuf = sendInitCmd(pBuf);
  }
}

void setWindows(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint16_t row_start, row_end;
  uint16_t col_start, col_end;
  col_start = x;
  col_end = x + w - 1;
  row_start = y;
  row_end =  y + h - 1;
  uint8_t buf[2];
  sendCmd(0x2A);
  buf[0] = col_start >> 8;
  buf[1] = col_start;
  sendData(buf, 2);
  buf[0] = col_end >> 8;
  buf[1] = col_end;
  sendData(buf, 2);
  sendCmd(0x2B);
  buf[0] = row_start >> 8;
  buf[1] = row_start;
  sendData(buf, 2);
  buf[0] = row_end >> 8;
  buf[1] = row_end;
  sendData(buf, 2);
  sendCmd(0x2C);
}

void setPixels(uint16_t *pixels, uint16_t sz)
{
  sendData((uint8_t*)pixels, sz * 2);
}

//******************************************************************************
// Name: mainApp_preHalInit()
// Params: void
// Return: void
// Brief: Init before HAL, just out of reset.
//******************************************************************************
void mainApp_preHalInit(void)
{

}

//******************************************************************************
//* Name:   mainApp_init()
//* Params: void
//* Return: void
//* Brief:  Initialization of main applicative.
//******************************************************************************
void mainApp_init(void)
{
//  drvLedD_init();
//  drvKpad_init();
//  cmpOled_init();
//  touchgfxIf_init();

  // Instantiate led driver.
//  instLedV = drvLedD_instantiate(&callbackGreenLed);

  drvTime_startTempo_ms(&tempo, 1000);

  initTft();
  drvTime_wait_ms(1000);
  uint16_t au16Buf[11];
  setWindows(10, 10, 11, 1);
  memset(au16Buf, 0xFF, sizeof(au16Buf));
  setPixels(au16Buf, sizeof(au16Buf) / 2);
  setWindows(20, 10, 21, 1);
  memset(au16Buf, 0x00, sizeof(au16Buf));
  setPixels(au16Buf, sizeof(au16Buf) / 2);
}

//******************************************************************************
// Name: mainApp_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void mainApp_task(void)
{
//  drvLedD_task();
//  drvKpad_task();
//  touchgfxIf_task();
//  cmpOled_taskTest();

//  drvLedD_toggle(0);

  if(drvTime_isElapsed(&tempo))
  {
    drvTime_startTempo_ms(&tempo, 10000);
    
  }

}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// Name: callbackGreenLed()
// Params: void
// Return: void
// Brief: callback called when LD2 (green led) is changing state.
//******************************************************************************
//void callbackGreenLed(bool ledOn) {
//  HAL_GPIO_WritePin(D13_GPIO_Port, D13_Pin, ledOn);
//}

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
