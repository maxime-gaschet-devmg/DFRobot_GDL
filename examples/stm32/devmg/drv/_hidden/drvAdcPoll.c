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
// Driver / Service: driver for ADC. It's a simple API.
// Use: drives simply HAL ADC: choose a channel and wait for conversion.
//  - Configure GPIOs wantend inputs as analog.
//  - Configure ADCx by enabling inputs, choosing clock prescaler, setting
//    resolution to 12bits, right alignment, disabling scan, continous,
//    discontinuous and DMA modes.
//  - This driver uses single regular conversion.
//  - Sample time and timeout are fixed values.
//******************************************************************************

// INCLUDES ********************************************************************

#include <drvAdcPoll.h>

// DEFINES *********************************************************************

// Default fixed values.
#define DEFAULT_SAMPLING_TIME (ADC_SAMPLETIME_640CYCLES_5)
#define DEFAULT_TIMEOUT_1MS (10)

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************
// FUNCTIONS *******************************************************************

//******************************************************************************
// Name: drvAdc_setChannel()
// Params: HAL handle of ADC and channel ADC_CHANNEL_x, see defgroup ADC_HAL_EC_CHANNEL.
// Return: true if configuration ok.
// Brief: configure ADC with channel in parameter.
//******************************************************************************
bool drvAdc_setChannel(ADC_HandleTypeDef *hadc, uint32_t u32Channel)
{
  ADC_ChannelConfTypeDef sConfig;
  sConfig.Channel = u32Channel;
  sConfig.Rank = ADC_REGULAR_RANK_1; // Only regular rank 1 is used in this driver.
  sConfig.SamplingTime = DEFAULT_SAMPLING_TIME;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  return (HAL_OK == HAL_ADC_ConfigChannel(hadc, &sConfig));
}

//******************************************************************************
// Name: drvAdc_convertSingle()
// Params: HAL handle of ADC.
// Return: value of conversion in 12b (0..4095) or 0xFFFF if error.
// Brief: start a single conversion, wait until conversion done or timeout (fixed
//  value) and return the value.
//******************************************************************************
uint16_t drvAdc_convertSingle(ADC_HandleTypeDef *hadc)
{
  return drvAdc_convertAverage(hadc, 1);
}

//******************************************************************************
// Name: drvAdc_convertSingle()
// Params: HAL handle of ADC, number of conversions to do.
// Return: average of conversion in 12b (0..4095) or 0xFFFF if error.
// Brief: start several conversions, wait until conversion done or timeout (fixed
//  value) and return the average value.
//******************************************************************************
uint16_t drvAdc_convertAverage(ADC_HandleTypeDef *hadc, uint8_t u8NumConv)
{
  uint16_t u16Ret = 0xFFFF;
  if(HAL_OK == HAL_ADC_Start(hadc))
  {
    uint32_t u32Accumulator = 0;
    uint8_t i = u8NumConv;
    while(i > 0)
    {
      if(HAL_OK == HAL_ADC_PollForConversion(hadc, DEFAULT_TIMEOUT_1MS)) {
        u32Accumulator += HAL_ADC_GetValue(hadc);
        i--;
      }
      else {
        break;
      }
    }
    if(0 == i) {
      u16Ret = (uint16_t)((u32Accumulator + u8NumConv / 2) / u8NumConv);
    }
  }
  HAL_ADC_Stop(hadc); // Required. Otherwise sometimes first conv is wrong.
  return u16Ret;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
