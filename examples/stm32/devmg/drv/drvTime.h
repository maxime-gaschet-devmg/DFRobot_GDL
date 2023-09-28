//******************************************************************************
//* Projet : 
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

#ifndef DRVTIME_H_
#define DRVTIME_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

  typedef struct
  {
    uint32_t u32StartTick;
    uint32_t u32DurationTicks;
    bool bRunning;
  } timeout_t;

  typedef struct
  {
    uint32_t u32StartTick;
    uint32_t u32DurationTicks;
    bool bRunning;
  } tempo_t; // Must be exactly same to timeout_t!
  // Note: we don't define as "typedef timeout_t tempo_t" because in case of
  //  mismatch between function & variable (for exemple drvTime_startTimeout_ms(pTempo))
  //  the compiler won't warn.

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  // Init / task.
  void drvTime_init(void);
  void drvTime_task(void);

  // Blocking tempo.
  void drvTime_wait_ms(uint32_t u32Wait_ms);
  void drvTime_wait_us(uint32_t u32Wait_us);
  void drvTime_wait_cy(uint32_t u32Wait_cy);

  // Timeout.
  void drvTime_startTimeout_ms(timeout_t *pTimeout, uint32_t u32Timeout_ms);
  void drvTime_stopTimeout(timeout_t *pTimeout);
  bool drvTime_isTimedOut(timeout_t *pTimeout);
  void drvTime_restartTimeout(timeout_t *pTimeout);
  uint32_t drvTime_getTimeElapsed_ms(timeout_t *pTimeout);
  uint32_t drvTime_getTimeLeft_ms(timeout_t *pTimeout);

  // Tempo.
  void drvTime_startTempo_ms(tempo_t *psTempo, uint32_t u32Tempo_ms);
  void drvTime_stopTempo(tempo_t *psTempo);
  bool drvTime_isElapsed(tempo_t *psTempo);
  void drvTime_restartTempo(tempo_t *psTempo);

  // GetTick.
  uint32_t drvTime_getTick_ms(void);

  // Callback registration.
  bool drvTime_registerCallback_ms(void (*pCallback)(void));
  bool drvTime_registerCallback_s(void (*pCallback)(void));

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
