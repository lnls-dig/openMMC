//
// This file is part of the GNU ARM Eclipse Plug-ins project.
// Copyright (c) 2014 Liviu Ionescu.
//

#include <stdint.h>

#include <stm32f30x.h>
#include <system_stm32f30x.h>
#include <core_cm4.h>

#include "stm32_timer.h"
//#include "cortexm/ExceptionHandlers.h"


// ----------------------------------------------------------------------------

// Forward declarations.

void
timer_tick (void);

// ----------------------------------------------------------------------------

volatile timer_ticks_t timer_delayCount;

// ----------------------------------------------------------------------------

void
timer_start (void)
{
  //printf("SystemCoreClock: %d\n\r", SystemCoreClock);
  // Use SysTick as reference for the delay loops.
  SysTick_Config (SystemCoreClock / TIMER_FREQUENCY_HZ);
}

void
timer_sleep (timer_ticks_t ticks)
{
  timer_delayCount = ticks;

  // Busy wait until the SysTick decrements the counter to zero.
  while (timer_delayCount != 0u)
    ;
}

void
timer_tick (void)
{
  // Decrement to zero the counter used by the delay routine.
  if (timer_delayCount != 0u)
    {
      --timer_delayCount;
    }
}

volatile int tc = 0;
// ----- SysTick_Handler() ----------------------------------------------------

void
zSysTick_Handler (void)
{
  tc++;
  timer_tick ();
}

// ----------------------------------------------------------------------------
