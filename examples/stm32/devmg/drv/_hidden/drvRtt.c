//******************************************************************************
//* Project : Remote
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
// Driver / Service: drvRtt
// Use: Driver of SEGGER RTT communication bus. It uses SWD/SWC with a JLINK.
//  Use Rtt viewer to receive and transmit frames to MCU.
//******************************************************************************

// INCLUDES ********************************************************************

#include <drvRtt.h>
#include <lib/_hidden/Segger/RTT/SEGGER_RTT.h>
#include "drvTime.h"

// DEFINES *********************************************************************

// RX
#define RX_BUF_SZ_U16 (128)
#define RX_TIMEOUT_1MS (1000) // 1s.

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

static struct {
  uint8_t au8Buf[RX_BUF_SZ_U16];
  timeout_t sTimeout;  // If timeout, RX buffer is flushed.
  uint16_t u16NbBytes;  // Number of bytes received so far.
} sRx;


// FUNCTIONS *******************************************************************

static void resetRxProcess(void);

//******************************************************************************
// Name: drvRtt_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvRtt_init(void)
{
  SEGGER_RTT_Init();

  // Init buffers.
  memset(&sRx, 0, sizeof(sRx));
}

//******************************************************************************
// Name: drvRtt_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvRtt_task(void)
{
  static uint32_t u32DataToRead;

  // Check if new data.
  u32DataToRead = SEGGER_RTT_HasData(0);
  if(u32DataToRead > 0)
  {
    if(u32DataToRead + sRx.u16NbBytes > RX_BUF_SZ_U16 - 1) { // -1 for null terminated string.
      resetRxProcess(); //  Rx overflow
    }
    else
    {
      static uint16_t u16Ind;
      drvTime_startTimeout_ms(&sRx.sTimeout, RX_TIMEOUT_1MS); // Start timeout.
      u16Ind = sRx.u16NbBytes;
      sRx.u16NbBytes += SEGGER_RTT_Read(0, &sRx.au8Buf[sRx.u16NbBytes],
                                        u32DataToRead); // Read data.
      // Must be printable and ended by a line feed.
      while(u16Ind < sRx.u16NbBytes)
      {
        if(isprint(sRx.au8Buf[u16Ind])) {
          u16Ind++;
        }
        else if(CHAR_LF == sRx.au8Buf[u16Ind])
        {
          // Pass string to callback.
          sRx.au8Buf[u16Ind] = 0; // End buf by a null character to make a string (overwrite LF).
          drvRtt_parseStringCallback((char*)sRx.au8Buf);
          sRx.u16NbBytes -= u16Ind + 1; // This is what left.
          memmove(sRx.au8Buf, &sRx.au8Buf[u16Ind + 1], sRx.u16NbBytes); // Copy what follows the string.
          sRx.sTimeout.bRunning = (sRx.u16NbBytes > 0); // Update timeout.
          break;
        }
        else {
          resetRxProcess();
        } // Invalid char => flush.
      } //end while.
    }
  } //end while.

  // Check if RX timeout.
  if((sRx.u16NbBytes > 0) && drvTime_isTimedOut(&sRx.sTimeout))
  {
    drvTime_stopTimeout(&sRx.sTimeout);
    resetRxProcess(); // Reset RX in case of timeout.
    SEGGER_RTT_WriteString(0, "\nDbg Rx timeout!");
  }

} //end fun.

//******************************************************************************
// Name: drvRtt_printf()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvRtt_printf(const char * strPrint, ...)
{
  va_list ParamList;
  va_start(ParamList, strPrint);
  SEGGER_RTT_vprintf(0, strPrint, &ParamList);
  va_end(ParamList);
}

//******************************************************************************
// Name: drvRtt_printHexBuf()
// Params: void
// Return: void
// Brief: Print a buffer to hex string. {16, 10} => "10 0A "
//******************************************************************************
void drvRtt_printHexBuf(uint8_t* pu8Buf, uint16_t u16Len)
{
  while(u16Len-- > 0) {
    drvRtt_printf("%02X ", *(pu8Buf++));
  }
}


//******************************************************************************
// Name: drvRtt_printStr()
// Params: void
// Return: void
// Brief: print a string.
//******************************************************************************
void drvRtt_printStr(char* str)
{
  SEGGER_RTT_WriteString(0, str);
}

//******************************************************************************
// Name: drvRtt_printBuf()
// Params: void
// Return: void
// Brief: print a buffer.
//******************************************************************************
void drvRtt_printBuf(uint8_t* buf, uint16_t u16Len)
{
  SEGGER_RTT_Write(0, buf, u16Len);
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: resetRxProcess()
// Params: void
// Return: void
// Brief: Reset the rx process: flush buffer, stop timeout and reset flags.
//******************************************************************************
void resetRxProcess(void)
{
  sRx.u16NbBytes = 0; // Flush buffer.
  sRx.sTimeout.bRunning = false; // Stop timeout.
  SEGGER_RTT_printf(0, "\nReset dbg rx process");
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// Name: drvRtt_parseStringCallback()
// Params: void
// Return: void
// Brief:
//******************************************************************************
__weak void drvRtt_parseStringCallback(char* str)
{
  // To be implemented in higher levels.
  UNUSED(str);

  SEGGER_RTT_printf(0, "\nReceived %s", str);
}

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************



// EOF *************************************************************************
