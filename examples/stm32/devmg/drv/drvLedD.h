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

#ifndef DRV_LED_D_H_
#define DRV_LED_D_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdint.h>
#include <stdbool.h>

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

// Init, task and instantiation.
void drvLedD_init(void);
void drvLedD_task(void);
uint8_t drvLedD_instantiate(void (*pCallbackWrPin)(bool ledOn));

// Configure blink & flash if default values are not suitable.
void drvLedD_configureBlink(uint8_t inst, bool bFast_nSlow, uint16_t on_ms,
                             uint16_t off_ms, int8_t nbRepeat);
void drvLedD_configureFlash(uint8_t inst, uint16_t on_ms, uint16_t off_ms,
                             uint16_t wait_ms, int8_t nbRepeat);

// Led commands.
void drvLedD_off(uint8_t inst);
void drvLedD_on(uint8_t inst);
void drvLedD_toggle(uint8_t inst);
void drvLedD_blink(uint8_t inst, bool bFast_nSlow);
void drvLedD_flash(uint8_t inst, uint8_t nbFlashs);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
