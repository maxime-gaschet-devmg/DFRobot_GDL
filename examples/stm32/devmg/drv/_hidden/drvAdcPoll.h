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

#ifndef DRV_ADC_POLL_H_
#define DRV_ADC_POLL_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "adc.h" // CubeMX generated.

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  bool drvAdc_setChannel(ADC_HandleTypeDef *hadc, uint32_t u32Channel);
  uint16_t drvAdc_convertSingle(ADC_HandleTypeDef *hadc);
  uint16_t drvAdc_convertAverage(ADC_HandleTypeDef *hadc, uint8_t u8NumConv);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
