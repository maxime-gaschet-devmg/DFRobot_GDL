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

#ifndef DRVDBNC_H_
#define DRVDBNC_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

typedef struct {
  uint8_t       num;    // Number of the instance.
  GPIO_TypeDef *port;   // Port of the debounced pin.
  uint32_t      pin;    // Number of the debounced pin.
  GPIO_PinState stDbnc; // Debounced state.
  GPIO_PinState stPrev; // Debouncing state.
  uint8_t       cnt;    // Counter for debouncing.
} dbnc_t;

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

// Init / task.
void drvDbnc_init(void);
void drvDbnc_task(void);

// Instance.
dbnc_t *drvDbnc_createInst(GPIO_TypeDef *port, uint32_t pin, GPIO_PinState stInit);

// Callback.
void drvDbnc_callback(dbnc_t *dbnc);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
