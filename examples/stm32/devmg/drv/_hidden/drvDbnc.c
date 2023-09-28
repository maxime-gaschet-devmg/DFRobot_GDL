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
// Driver / Service: drvDbnc - driver for debouncer.
// Use: Driver that debounces hardware pushbutton inputs.
//******************************************************************************

// INCLUDES ********************************************************************

#include "drvDbnc.h"

#include "drvTime.h"
#include "drvRtt.h" // For debug purpose.

// DEFINES *********************************************************************

// Maximum number of driver instances.
#define MAX_INST_NUM (2)

// Period of task process.
#define TASK_PERIOD_MS (10)

// Count required to validate debounce.
#define CNT_DBNC ((uint8_t)3) // 3 x 10ms = 30ms.

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

static dbnc_t aDbncPoll[MAX_INST_NUM];    // Static poll of drivers.
static uint8_t instNum;                   // Number of instances.
static tempo_t tempoTask;                 // Tempo between process of task.

// FUNCTIONS *******************************************************************

// Private.
static void processTask(dbnc_t *dbnc);

//******************************************************************************
// Name: drvDbnc_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvDbnc_init(void)
{
  // Init vars.
  memset(aDbncPoll, 0, sizeof(aDbncPoll));
  instNum = 0;
  drvTime_startTempo_ms(&tempoTask, TASK_PERIOD_MS); // Process task every xms.
}

//******************************************************************************
// Name: drvDbnc_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvDbnc_task(void)
{
  if(drvTime_isElapsed(&tempoTask))
  {
    drvTime_restartTempo(&tempoTask);

    // Process debouncers.
    static uint8_t i;
    for(i = 0; i < instNum; i++) {
      processTask(&aDbncPoll[i]);
    }
  }
}

//******************************************************************************
// Name: drvDbnc_createInst()
// Params: port, pin and init state of the debounced input.
// Return: pointer to the driver if instance created or null.
// Brief: create instance of a debounce driver.
//******************************************************************************
dbnc_t *drvDbnc_createInst(GPIO_TypeDef *port, uint32_t pin, GPIO_PinState stInit)
{
  dbnc_t *dbnc = NULL;
  if(instNum < MAX_INST_NUM)
  {
    dbnc = &aDbncPoll[instNum];
    dbnc->num = instNum;
    dbnc->port = port;
    dbnc->pin = pin;
    dbnc->stDbnc = stInit;
    dbnc->stPrev = stInit;
    instNum++;
  }
  return dbnc;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: processTask()
// Params: pointer to the debounce driver.
// Return: void
// Brief: process a debounce driver task.
//******************************************************************************
void processTask(dbnc_t *dbnc)
{
  static GPIO_PinState stNew;
  stNew = HAL_GPIO_ReadPin(dbnc->port, dbnc->pin);
  if(dbnc->stPrev != stNew)
  {
    // State just changed, reset counter.
    dbnc->stPrev = stNew;
    dbnc->cnt = 0;
  }
  else
  {
    if(dbnc->stDbnc != dbnc->stPrev)
    {
      if(dbnc->cnt < CNT_DBNC){
        dbnc->cnt++;
      }
      else {
        dbnc->stDbnc = dbnc->stPrev;
        drvDbnc_callback(dbnc);
      }
    }
  }
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// Name: drvDbnc_callback()
// Params: pointer to the debounce driver.
// Return: void
// Brief: called on every transition of the debounced state (stDbnc).
//******************************************************************************
__weak void drvDbnc_callback(dbnc_t *dbnc)
{
  // To be implemented in higher level.
  drvRtt_printf("\n Dbnc%d state=%d", dbnc->num, dbnc->stDbnc);
}

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************



// EOF *************************************************************************
