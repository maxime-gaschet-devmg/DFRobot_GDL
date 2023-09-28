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
// Driver / Service: driver for time, usefull for timeout & tempo.
//  Compatible with Arduino & CubeIde.
// Use:
//******************************************************************************

// INCLUDES ********************************************************************

#include <drvTime.h>

#if defined ARDUINO
  #include <Arduino.h>
#elif defined USE_HAL_DRIVER
  #include "tim.h"
#else
  #error platform not supported.
#endif

// DEFINES *********************************************************************

#define MAX_NB_CALLBACKS (5)

#ifdef USE_HAL_DRIVER
  #define LOCAL_HANDLE (&htim6) // Should be a timer configured at 1MHz tick.
#endif

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

// Private
static void (*apCallbacks_ms[MAX_NB_CALLBACKS]) (void);  // Array of callbacks registered to 1ms elapsed.
static void (*apCallbacks_s[MAX_NB_CALLBACKS]) (void); // Array of callbacks registered to 1s elapsed.
static uint8_t numCallbacks_ms;
static uint8_t numCallbacks_s;
static uint32_t prevTick;

// FUNCTIONS *******************************************************************

// Private
uint32_t getNbTicksElapsed(timeout_t* psTimeout);

//******************************************************************************
// Name: drvTime_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvTime_init(void)
{
  memset(apCallbacks_ms, 0, sizeof(apCallbacks_ms));
  memset(apCallbacks_s, 0, sizeof(apCallbacks_s));
  numCallbacks_ms = 0;
  numCallbacks_s = 0;
  prevTick = 0;
}

//******************************************************************************
// Name: drvTime_task()
// Params: void
// Return: void
// Brief: task of the time driver.
//******************************************************************************
void drvTime_task(void)
{
  static uint32_t newTick;
  static uint8_t i;

  newTick = drvTime_getTick_ms();
  if(prevTick != newTick)
  {
    // 1ms elapsed => call callbacks 1ms.
    for(i = 0; i < numCallbacks_ms; i++) {
      (*apCallbacks_ms[i])();
    }

    if((prevTick % 1000) > (newTick % 1000))
    {
      // 1sec elapsed => call callbacks 1sec.
      for(i = 0; i < numCallbacks_s; i++) {
        (*apCallbacks_s[i])();
      }
    }
    prevTick = newTick;
  }
}

//******************************************************************************
// Name: drvTime_wait_cy()
// Params: time to wait in CPU cycles.
// Return: void
// Brief: blocking function waiting for AT LEAST x CPU cycles but returning.
//******************************************************************************
void drvTime_wait_cy(uint32_t u32Wait_cy)
{
  volatile uint32_t u32Cnt;
  u32Cnt = u32Wait_cy;
  while(u32Cnt > 0)
    u32Cnt--;
}

//******************************************************************************
// Name: drvTime_wait_us()
// Params: time to wait in microseconds.
// Return: void
// Brief: blocking function waiting AT LEAST x us before leaving.
//******************************************************************************
void drvTime_wait_us(uint32_t u32Wait_us)
{
  if(u32Wait_us > 0)
  {
  #ifdef ARDUINO
    delayMicroseconds(u32Wait_us);
  #else // USE_HAL_DRIVER
    HAL_TIM_Base_Start(LOCAL_HANDLE);
    LOCAL_HANDLE->Instance->CNT = 0;
    while(LOCAL_HANDLE->Instance->CNT <= u32Wait_us);
  #endif
  }
}

//******************************************************************************
// Name: drvTime_wait_ms()
// Params: time to wait in milliseconds.
// Return: void
// Brief: blocking function waiting x ms (-1/+0 ms) before leaving.
//******************************************************************************
void drvTime_wait_ms(uint32_t u32Wait_ms)
{
  timeout_t sTimeout;
  drvTime_startTimeout_ms(&sTimeout, u32Wait_ms);
  while(!drvTime_isTimedOut(&sTimeout))
    {;}
}

//******************************************************************************
// Name: drvTime_startTimeout_ms()
// Params: sTimeout_t* psTimeout, uint32_t u32Timeout1ms
// Return: void
// Brief: start a timeout with the duration in parameter.
//******************************************************************************
void drvTime_startTimeout_ms(timeout_t* psTimeout, uint32_t u32Timeout_1ms)
{
  // 1 tick = 1 ms.
  psTimeout->u32StartTick = drvTime_getTick_ms();
  psTimeout->u32DurationTicks = u32Timeout_1ms;
  psTimeout->bRunning = true;
}

//******************************************************************************
// Name: drvTime_stopTimeout()
// Params: sTimeout_t* psTimeout
// Return: void
// Brief: stop the timeout.
//******************************************************************************
void drvTime_stopTimeout(timeout_t* psTimeout) {
  psTimeout->bRunning = false;
}

//******************************************************************************
// Name: drvTime_isTimedOut()
// Params: sTimeout_t* psTimeout
// Return: void
// Brief: return true if timeout.
//******************************************************************************
bool drvTime_isTimedOut(timeout_t* psTimeout)
{
  if(psTimeout->bRunning)
  {
    if(getNbTicksElapsed(psTimeout) >= psTimeout->u32DurationTicks)
    {
      psTimeout->bRunning = false;
      return true;
    }
    else
      return false;
  }
  else
    return true;
}

//******************************************************************************
// Name: SrvTime_restartTimeout()
// Params: sTimeout_t* psTimeout
// Return: void
// Brief: restart the timeout.
//******************************************************************************
void drvTime_restartTimeout(timeout_t* psTimeout) {
  drvTime_startTimeout_ms(psTimeout, psTimeout->u32DurationTicks);
}

//******************************************************************************
// Name: drvTime_getTimeElapsed_1ms()
// Params: sTimeout_t* psTimeout
// Return: void
// Brief: return the elapsed time in ms since the timeout start.
//******************************************************************************
uint32_t drvTime_getTimeElapsed_ms(timeout_t* psTimeout) {

  return getNbTicksElapsed(psTimeout); // 1 tick = 1 ms.
}

//******************************************************************************
// Name: drvTime_getTimeLeft_ms()
// Params: sTimeout_t* psTimeout
// Return: uint32_t
// Brief: return the left time in ms.
//******************************************************************************
uint32_t drvTime_getTimeLeft_ms(timeout_t* psTimeout) {
  return psTimeout->u32DurationTicks - getNbTicksElapsed(psTimeout); // 1 tick = 1 ms.
}

//******************************************************************************
// Name: drvTime_startTempo_ms()
// Params: pointer to tempo, tempo duration in ms.
// Return: void
// Brief: start the tempo with the duration in parameter.
//******************************************************************************
void drvTime_startTempo_ms(tempo_t* psTempo, uint32_t u32Tempo_1ms) {
  drvTime_startTimeout_ms((timeout_t*)psTempo, u32Tempo_1ms);
}

//******************************************************************************
// Name: drvTime_stopTempo()
// Params: pointer to tempo.
// Return: void
// Brief: stop the tempo.
//******************************************************************************
void drvTime_stopTempo(tempo_t* psTempo) {
  drvTime_stopTimeout((timeout_t*)psTempo);
}

//******************************************************************************
// Name: drvTime_isElapsed()
// Params: pointer to tempo.
// Return: true if tempo elapsed.
// Brief: return true if tempo elapsed.
//******************************************************************************
bool drvTime_isElapsed(tempo_t* psTempo) {
  return drvTime_isTimedOut((timeout_t*)psTempo);
}

//******************************************************************************
// Name: drvTime_restartTempo()
// Params: tempo_t* psTempo
// Return: void
// Brief: redemarre une tempo.
//******************************************************************************
void drvTime_restartTempo(tempo_t* psTempo) {
  drvTime_restartTimeout((timeout_t*)psTempo);
}

//******************************************************************************
// Name: drvTime_getTick_ms()
// Params: void
// Return: return the current tick in ms.
// Brief: redemarre une tempo.
//******************************************************************************
uint32_t drvTime_getTick_ms(void)
{
  #ifdef ARDUINO
    return millis();
  #else // USE_HAL_DRIVER
    return HAL_GetTick();
  #endif
}

//******************************************************************************
// Name: drvTime_registerCallback_1ms()
// Params: pCallback: pointer to callback.
// Return: void
// Brief:
//******************************************************************************
bool drvTime_registerCallback_ms(void (*pCallback)(void))
{
  if(numCallbacks_ms < MAX_NB_CALLBACKS) {
    apCallbacks_ms[numCallbacks_ms++] = pCallback;
    return true;
  }
  return false;
}

//******************************************************************************
// Name: drvTime_registerCallback_1sec()
// Params: pCallback: pointer to callback.
// Return: void
// Brief:
//******************************************************************************
bool drvTime_registerCallback_s(void (*pCallback)(void))
{
  if(numCallbacks_s < MAX_NB_CALLBACKS) {
    apCallbacks_s[numCallbacks_s++] = pCallback;
    return true;
  }
  return false;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: getNbTicksElapsed()
// Params: pointer to timeout.
// Return: void
// Brief: return the elapsed time in ticks.
//******************************************************************************
uint32_t getNbTicksElapsed(timeout_t* psTimeout)
{
  uint32_t u32NbTicksElapsed;

  // Si le timeout est en cours, on regarde le temps ecoule depuis son demarrage.
  // Si le temps ecoule est superieur a sa duree, le timeout est ecoule.
  if(psTimeout->bRunning)
  {
    u32NbTicksElapsed = drvTime_getTick_ms() - psTimeout->u32StartTick;
    if(u32NbTicksElapsed >= psTimeout->u32DurationTicks)
      psTimeout->bRunning = false;
  }

  // Si le timeout est ecoule, on renvoie sa duree, sinon on renvoie le temps
  //  ecoule.
  if(!psTimeout->bRunning)
    return u32NbTicksElapsed = psTimeout->u32DurationTicks;

  return u32NbTicksElapsed;
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
