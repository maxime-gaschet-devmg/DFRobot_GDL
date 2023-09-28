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
// Driver / Service: driver to drive PWM. It's a simple API driver.
// Use: configure, start and stop PWM outputs. PWM frequency must be set in
//  cubeMx (see calculation below), PWM duty can be changed on the fly using
//  either the counter, a percent or a permille.
//  - Choose a timer that can drive the desired output pins.
//  - Select clock source as internal.
//  - Select one or severals channels of this timer as PWM generation. Be advised
//    all channels will have the same frequency.
//  - Disable One Pulse mode.
//  - Reset configuration of the timer.
//  - Choose a prescaler and counter period to achieve the desired frequency.
//      FreqPwm = APBx_timer / ((1 + prescaler) * (1 + counter_period)).
//  - Refer to "Memory map and register boundary addresses" paragraph of datasheet
//    to know which APBx is applicable to the selected timer.
//******************************************************************************

// INCLUDES ********************************************************************

#include "drvPwm.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************
// FUNCTIONS *******************************************************************

//******************************************************************************
// Name: drvPwm_setDutyCounter()
// Params: pointer to HAL timer handle, desired channel TIM_CHANNEL_x and the
//  dutyCounter.
// Return: void
// Brief: set duty cycle using counter period. For example, if counter period
//  (or autoreload aka ARR) is 1000 and dutyCounter is 250, the resulting duty
//  cycle is 25%.
//******************************************************************************
void drvPwm_setDutyCounter(TIM_HandleTypeDef *htim, uint32_t channel,
                           uint32_t dutyCounter)
{
  __HAL_TIM_SetCompare(htim, channel, dutyCounter);
}

//******************************************************************************
// Name: drvPwm_setDutyPercent()
// Params: pointer to HAL timer handle, desired channel TIM_CHANNEL_x and duty
//  cycle in percent.
// Return: void
// Brief: set duty cycle using percent. Example: 25%.
//******************************************************************************
void drvPwm_setDutyPercent(TIM_HandleTypeDef *htim, uint32_t channel,
                           uint8_t dutyPercent)
{
  uint32_t dutyCounter;
  dutyCounter = (((__HAL_TIM_GetAutoreload(htim) + 1) * dutyPercent) + 50) / 100;
  __HAL_TIM_SetCompare(htim, channel, dutyCounter);
}

//******************************************************************************
// Name: drvPwm_setDutyPermille()
// Params: pointer to HAL timer handle, desired channel TIM_CHANNEL_x and duty
//  cycle in permille.
// Return: void
// Brief: set duty cycle using permille. Example: 250‰.
//******************************************************************************
void drvPwm_setDutyPermille(TIM_HandleTypeDef *htim, uint32_t channel,
                            uint16_t dutyPermille)
{
  uint32_t dutyCounter;
  dutyCounter = (((__HAL_TIM_GetAutoreload(htim) + 1) * dutyPermille) + 500) / 1000;
  __HAL_TIM_SetCompare(htim, channel, dutyCounter);
}

//******************************************************************************
// Name: drvPwm_enable()
// Params: pointer to HAL timer handle and desired channel TIM_CHANNEL_x.
// Return: void
// Brief: enable / disable the PWM of which channel and timer is in parameters.
//******************************************************************************
void drvPwm_enable(TIM_HandleTypeDef *htim, uint32_t channel, bool enable)
{
  if(enable) {
    HAL_TIM_PWM_Start(htim, channel);
  }
  else {
    HAL_TIM_PWM_Stop(htim, channel);
  }
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
