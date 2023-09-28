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
// Driver / Service: driver for debug led.
// Use: each driver can drive a single led with fix on, blinking or flash trains.
//  - Configure output pins using CubeMx.
//  - Place init() at end of HAL initialization.
//  - Place task() in the loop.
//  - Instantiate and keep record of the instance number. It requires a definition
//  of the write pin callback.
//  - Call a led cmd with the instance number.
//******************************************************************************

// INCLUDES ********************************************************************

#include <drvLedD.h>
#include <drvTime.h>

// DEFINES *********************************************************************

// Maximum number of driver instances.
#define MAX_INSTANCES_NB (3)

// TYPEDEFS ********************************************************************

// Command definitions.
typedef enum {
	cmd_off = 0,
	cmd_on = 1,
	cmd_blinkFast = 2,
	cmd_blinkSlow = 3,
	cmd_flash = 4,
} cmd_t;

// State definition.
typedef enum {
	st_off,
	st_on,
} st_t;

// Structure of driver.
typedef struct {
  uint8_t inst;                       // Number of instance.
  void (*pCallbackWrPin)(bool);       // Callback to write pin.
  cmd_t cmd;                          // Current command.
  st_t st;                            // Current state.
  int16_t cyclesLeft;                 // Number of left cycles.
  uint8_t flashCnt;                   // Counts of flash.
  tempo_t tempo;
  uint16_t param_blinkFast_on_ms;     // On time for fast blink.
  uint16_t param_blinkFast_off_ms;    // Off time for fast blink.
  int8_t   param_blinkFast_repeat;    // Number of repeats for fast blink.
  uint16_t param_blinkSlow_on_ms;     // On time for slow blink.
  uint16_t param_blinkSlow_off_ms;    // Off time for slow blink.
  int8_t   param_blinkSlow_repeat;    // Number of repeats for slow blink.
  uint16_t param_flash_on_ms;         // On time for a flash.
  uint16_t param_flash_off_ms;        // Off time for a flash.
  uint16_t param_flash_wait_ms;       // Wait time between flash trains.
  int8_t   param_flash_repeat;        // Number of repeats of flash trains.
  uint8_t  param_flash_nb;            // Number of flashes in a train.
} drv_t;

// VARIABLES *******************************************************************

static uint8_t nbInst;                 // Number of instances used.
static drv_t asPool[MAX_INSTANCES_NB]; // Pool of drivers.

// FUNCTIONS *******************************************************************

// Private.
void processTask(drv_t *pDrv);
drv_t *getDrv(uint8_t inst);

//******************************************************************************
// Name: drvLedD_init()
// Params: void
// Return: void
// Brief: init of debug led driver. This function should be called once right
//  after MCU boot up.
//******************************************************************************
void drvLedD_init(void)
{
  // Init driver pool.
  nbInst = 0;
  memset(asPool, 0, sizeof(asPool));
  for(int i = 0; i < MAX_INSTANCES_NB; i++) {
    asPool[i].inst = i;
  }
}

//******************************************************************************
// Name: drvLedD_task()
// Params: void
// Return: void
// Brief: maintain task of driver.
//******************************************************************************
void drvLedD_task(void)
{
  static uint8_t i;
  for(i = 0; i < nbInst; i++) {
    processTask(&asPool[i]);
  }
}

//******************************************************************************
// Name: drvLedD_instantiate()
// Params: pointer to led write pin callback.
// Return: void
// Brief: instantiate a driver. It needs a callback to write the led pin.
//******************************************************************************
uint8_t drvLedD_instantiate(void (*pCallbackWrPin)(bool))
{
  uint8_t inst = 0;
  if(nbInst < MAX_INSTANCES_NB) // Search for room in pool.
  {
    inst = nbInst++;
    drv_t *pDrv = getDrv(inst); // Retrieve pointer from pool.

    // Init instance.
    memset(pDrv, 0, sizeof(drv_t));
    pDrv->inst = nbInst;
    pDrv->pCallbackWrPin = pCallbackWrPin;
    pDrv->cmd = cmd_off;
    pDrv->st = st_off;
    pDrv->param_blinkFast_on_ms = 100;
    pDrv->param_blinkFast_off_ms = 100;
    pDrv->param_blinkFast_repeat = -1;
    pDrv->param_blinkSlow_on_ms = 1000;
    pDrv->param_blinkSlow_off_ms = 1000;
    pDrv->param_blinkSlow_repeat = -1;
    pDrv->param_flash_on_ms = 50;
    pDrv->param_flash_off_ms = 500;
    pDrv->param_flash_wait_ms = 0;
    pDrv->param_flash_repeat = 0;
    pCallbackWrPin(false);
  }
  return inst;
}

//******************************************************************************
// Name: drvLedD_configureBlink()
// Params: number of instance, boolean for fast or slow definition, duration of
//  on time in ms, duration of off time in ms, number of repeats - repeat can be
//  0 (single blink), 1 (one blink and one repeat) ..., or -1 for infinite repeat.
// Return: void
// Brief: configure blink timings and repeat if default is not suitable.
//******************************************************************************
void drvLedD_configureBlink(uint8_t inst, bool bFast_nSlow, uint16_t on_ms,
                             uint16_t off_ms, int8_t nbRepeat)
{
  drv_t *pDrv = getDrv(inst);
  if(bFast_nSlow){
    pDrv->param_blinkFast_on_ms = on_ms;
    pDrv->param_blinkFast_off_ms = off_ms;
    pDrv->param_blinkFast_repeat = nbRepeat;
  }
  else {
    pDrv->param_blinkSlow_on_ms = on_ms;
    pDrv->param_blinkSlow_off_ms = off_ms;
    pDrv->param_blinkSlow_repeat = nbRepeat;
  }
}

//******************************************************************************
// Name: drvLedD_configureFlash()
// Params: number of instance, duration of flash on time in ms, duration of flash
//  off time in ms, number of repeats of flash train - repeat can be 0 (single
//  flash train), 1 (one flash train and one repeat) ..., or -1 for infinite repeat.
// Return: void
// Brief: configure flash train timings and repeat if default is not suitable.
//******************************************************************************
void drvLedD_configureFlash(uint8_t inst, uint16_t on_ms, uint16_t off_ms,
                             uint16_t wait_ms, int8_t nbRepeat)
{
  drv_t *pDrv = getDrv(inst);
  pDrv->param_flash_on_ms = on_ms;
  pDrv->param_flash_off_ms = off_ms;
  pDrv->param_flash_wait_ms = wait_ms;
  pDrv->param_flash_repeat = nbRepeat;
}

//******************************************************************************
// Name: drvLedD_off()
// Params: number of instance.
// Return: void
// Brief: light off led.
//******************************************************************************
void drvLedD_off(uint8_t inst)
{
  drv_t *pDrv = getDrv(inst);
  pDrv->pCallbackWrPin(false);
  pDrv->cmd = cmd_off;
}

//******************************************************************************
// Name: drvLedD_on()
// Params: number of instance.
// Return: void
// Brief: light on led.
//******************************************************************************
void drvLedD_on(uint8_t inst)
{
  drv_t *pDrv = getDrv(inst);
  pDrv->pCallbackWrPin(true);
  pDrv->cmd = cmd_on;
}

//******************************************************************************
// Name: drvLedD_toggle()
// Params: number of instance.
// Return: void
// Brief: light on led.
//******************************************************************************
void drvLedD_toggle(uint8_t inst)
{
  drv_t *pDrv = getDrv(inst);
  pDrv->cmd = (cmd_off == pDrv->cmd ? cmd_on : cmd_off);
  pDrv->pCallbackWrPin(cmd_on == pDrv->cmd);
}

//******************************************************************************
// Name: drvLedD_blink()
// Params: number of instance, boolean for fast (true) or slow (false) configuration.
// Return: void
// Brief: blink led with either fast or slow configuration.
//******************************************************************************
void drvLedD_blink(uint8_t inst, bool bFast_nSlow)
{
  uint32_t duration;
  drv_t *pDrv = getDrv(inst);
  pDrv->pCallbackWrPin(true);
  pDrv->st = st_on;
  if(bFast_nSlow) {
    duration = pDrv->param_blinkFast_on_ms;
    pDrv->cmd = cmd_blinkFast;
    pDrv->cyclesLeft = pDrv->param_blinkFast_repeat;
  }
  else {
    duration = pDrv->param_blinkSlow_on_ms;
    pDrv->cmd = cmd_blinkSlow;
    pDrv->cyclesLeft = pDrv->param_blinkSlow_repeat;
  }
  drvTime_startTempo_ms(&pDrv->tempo, duration);
}

//******************************************************************************
// Name: drvLedD_flash()
// Params: number of instance, number of flashes in a train.
// Return: void
// Brief: light on led with a flash train.
//******************************************************************************
void drvLedD_flash(uint8_t inst, uint8_t nbFlashs)
{
  drv_t *pDrv = getDrv(inst);
  pDrv->pCallbackWrPin(true);
  pDrv->st = st_on;
  pDrv->param_flash_nb = nbFlashs;
  pDrv->cmd = cmd_flash;
  pDrv->cyclesLeft = nbFlashs * (pDrv->param_flash_repeat + 1 ) - 1;
  pDrv->flashCnt = 0;
  drvTime_startTempo_ms(&pDrv->tempo, pDrv->param_flash_on_ms);
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: processTask()
// Params: pointer to driver.
// Return: void
// Brief: process the driver's task in input.
//******************************************************************************
void processTask(drv_t *pDrv)
{
  // Task is necessary only with blink and flash.
  if( (pDrv->cmd >= cmd_blinkFast) && drvTime_isElapsed(&pDrv->tempo))
  {
    uint32_t duration_ms;
    if(st_off == pDrv->st)
    {
      // If state is off, light on led.
      pDrv->pCallbackWrPin(true);
      pDrv->st = st_on;
      switch(pDrv->cmd) {
        default:
        case cmd_blinkFast: duration_ms = pDrv->param_blinkFast_on_ms; break;
        case cmd_blinkSlow: duration_ms = pDrv->param_blinkSlow_on_ms; break;
        case cmd_flash:     duration_ms = pDrv->param_flash_on_ms; break;
      }
      drvTime_startTempo_ms(&pDrv->tempo, duration_ms);
    }
    else // if(st_on == pDrv->st)
    {
      // If state is on, check if there is a left cycle.
      pDrv->pCallbackWrPin(false);
      pDrv->st = st_off;
      if(pDrv->cyclesLeft != 0)
      {
        switch(pDrv->cmd) {
          default:
          case cmd_blinkFast: duration_ms = pDrv->param_blinkFast_off_ms; break;
          case cmd_blinkSlow: duration_ms = pDrv->param_blinkSlow_off_ms; break;
          case cmd_flash:
            // Differentiate off duration (between flashes) of wait duration
            //  (between trains).
            duration_ms = (++pDrv->flashCnt % pDrv->param_flash_nb ? pDrv->param_flash_off_ms : pDrv->param_flash_wait_ms);
            break;
        }
        drvTime_startTempo_ms(&pDrv->tempo, duration_ms);
        if(pDrv->cyclesLeft > 0) {
          pDrv->cyclesLeft--; // Cycle done.
        }
      }
      else {
        pDrv->cmd = cmd_off; // Nothing more to do.
      }
    }
  }
}

//******************************************************************************
// Name: getDrv()
// Params: number of instance.
// Return: void
// Brief: return pointer to driver in input.
//******************************************************************************
drv_t *getDrv(uint8_t inst) {
  return &asPool[inst];
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
