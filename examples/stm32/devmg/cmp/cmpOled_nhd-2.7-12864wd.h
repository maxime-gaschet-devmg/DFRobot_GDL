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

#ifndef CMPOLED_NHD_2_7_12864WD_H_
#define CMPOLED_NHD_2_7_12864WD_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************
#include <stdint.h>
// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  void cmpOled_init(void);
  void cmpOled_taskTest(void);
  void cmpOled_setDisplayWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
  void cmpOled_writeData(uint8_t *data, uint8_t length);
  void cmpOled_writeData_1bpp(uint8_t *pixels, uint8_t length);
  void cmpOled_writeData_4bpp(uint8_t *pixels, uint8_t length);
  void cmpOled_writeData_16bpp(uint16_t *pixels, uint8_t length);


#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
