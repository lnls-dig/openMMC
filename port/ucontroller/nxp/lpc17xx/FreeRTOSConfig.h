/*
   FreeRTOS V6.0.0 - Copyright (C) 2009 Real Time Engineers Ltd.

   This file is part of the FreeRTOS distribution.

   FreeRTOS is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License (version 2) as published by the
   Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 ***NOTE*** The exception to the GPL is included to allow you to distribute
 a combined work that includes FreeRTOS without being obliged to provide the
 source code for proprietary components outside of the FreeRTOS kernel.
 FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details. You should have received a copy of the GNU General Public
 License and the FreeRTOS license exception along with FreeRTOS; if not it
 can be viewed here: http://www.freertos.org/a00114.html and also obtained
 by writing to Richard Barry, contact details for whom are available on the
 FreeRTOS WEB site.

 1 tab == 4 spaces!

http://www.FreeRTOS.org - Documentation, latest information, license and
contact details.

http://www.SafeRTOS.com - A version that is certified for use in safety
critical systems.

http://www.OpenRTOS.com - Commercial support, development, porting,
licensing and training services.
 */

/******************************************************************************
  See http://www.freertos.org/a00110.html for an explanation of the
  definitions contained in this file.
 ******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configMAX_PRIORITIES                    ( 6 )
#define configUSE_TICK_HOOK                     0
#if defined(CHIP_LPC175X_6X)
#define configCPU_CLOCK_HZ                      ( ( unsigned long ) 8000000 )
#else
#define configCPU_CLOCK_HZ                      ( ( unsigned long ) 80000000 ) // OSC (8 MHz) -> PLL0 (80 MHz) -> CPU CLK
#endif
#define configTICK_RATE_HZ                      ( ( portTickType ) 1000 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 80 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 0x4000 ) )
#define configMAX_TASK_NAME_LEN                 ( 12 )
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_CO_ROUTINES                   0
#define configUSE_MUTEXES                       1
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_ALTERNATIVE_API               0
#define configCHECK_FOR_STACK_OVERFLOW          1
#define configUSE_RECURSIVE_MUTEXES             0
#define configQUEUE_REGISTRY_SIZE               3
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configENABLE_BACKWARD_COMPATIBILITY     1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#define configAPPLICATION_ALLOCATED_HEAP        1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               5
#define configTIMER_QUEUE_LENGTH                2
#define configTIMER_TASK_STACK_DEPTH            128

void vAssertCalled( char* file, uint32_t line);
#define configASSERT( x )     if( ( x ) == 0 ) { vAssertCalled( __FILE__, __LINE__ );}

#if (configGENERATE_RUN_TIME_STATS == 1)
#include "chip_lpc175x_6x.h"
extern void vConfigureTimerForRunTimeStats( void );
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE() LPC_TIMER0->TC
#endif

/* Set the following definitions to 1 to include the API function, or zero
   to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               0
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetCurrentTaskHandle       1

/* Use the system definition, if there is one */
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS       __NVIC_PRIO_BITS
#else
#define configPRIO_BITS                         5   /* 32 priority levels */
#endif

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY         ( 31 << (8 - configPRIO_BITS) )
/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 5 << (8 - configPRIO_BITS) )

/*
 * Use the Cortex-M3 optimisations, rather than the generic C implementation.
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* Dimensions a buffer that can be used by the FreeRTOS+CLI command
 interpreter.  Set this value to 1 to save RAM if FreeRTOS+CLI does not supply
 the output butter.  See the FreeRTOS+CLI documentation for more information:
 http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/ */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE     1024

#endif /* FREERTOS_CONFIG_H */

