//******************************************************************************
//* Projet : Remote
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

#ifndef DRVRTT_H_
#define DRVRTT_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  void drvRtt_init(void);
  void drvRtt_task(void);
  void drvRtt_printf(const char * sFormat, ...);
  void drvRtt_printHexBuf(uint8_t* pu8Buf, uint16_t u16Len);
  void drvRtt_printStr(char* str);
  void drvRtt_printBuf(uint8_t *buf, uint16_t u16Len);
  void drvRtt_parseStringCallback(char* str);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
