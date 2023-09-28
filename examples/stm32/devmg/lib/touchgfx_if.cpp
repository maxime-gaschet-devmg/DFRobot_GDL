//******************************************************************************
//* Project :  ceva_oled_imvac_touchgfx
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
// Driver / Service: touchgfx interface.
// Use:
//******************************************************************************

// INCLUDES ********************************************************************

#include <touchgfx/hal/OSWrappers.hpp>

#include "drvTime.h"

#include "drvLedD.h"

using namespace touchgfx;

// DEFINES *********************************************************************

#define FRAMERATE_PERIOD_MS (55)

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

static tempo_t tempo;

// FUNCTIONS *******************************************************************


//******************************************************************************
// Name: touchgfxIf_init()
// Params: void
// Return: void
// Brief: init interface with touchgfx.
//******************************************************************************
extern "C" void touchgfxIf_init(void)
{
  drvTime_startTempo_ms(&tempo, FRAMERATE_PERIOD_MS);
}

//******************************************************************************
// Name: touchgfxIf_task()
// Params: void
// Return: void
// Brief: wrap in C domain the VSYNC signal of touchgfx. This function calls
//  periodically at the period FRAMERATE_PERIOD_MS.
//******************************************************************************
extern "C" void touchgfxIf_task(void)
{
  if(drvTime_isElapsed(&tempo))
  {
    drvTime_restartTempo(&tempo);
    OSWrappers::signalVSync();
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
