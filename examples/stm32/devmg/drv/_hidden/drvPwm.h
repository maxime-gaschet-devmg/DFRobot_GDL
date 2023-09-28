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

#ifndef DRV_PWM_H_
#define DRV_PWM_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "tim.h" // CubeMx generated.

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  void drvPwm_setDutyCounter(TIM_HandleTypeDef *htim, uint32_t channel,
                             uint32_t dutyCounter);
  void drvPwm_setDutyPercent(TIM_HandleTypeDef *htim, uint32_t channel,
                             uint8_t dutyPercent);
  void drvPwm_setDutyPermille(TIM_HandleTypeDef *htim, uint32_t channel,
                              uint16_t dutyPermille);
  void drvPwm_enable(TIM_HandleTypeDef *htim, uint32_t channel, bool enable);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
