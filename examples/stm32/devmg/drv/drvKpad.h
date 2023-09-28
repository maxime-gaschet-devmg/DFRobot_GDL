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

#ifndef DRVKPAD_H_
#define DRVKPAD_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdbool.h>

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

  typedef enum {
    key_none,
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,
    key_0,
    key_A,
    key_B,
    key_C,
    key_D,
    key_star,  // *
    key_pound, // #
  } keyPad_t;

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  void drvKpad_init();
  void drvKpad_task();
  bool drvKpad_getKeyPressed(keyPad_t *key);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
