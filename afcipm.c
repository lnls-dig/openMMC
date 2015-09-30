/*
 * AFC_IPM.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Project includes */
#include "chip.h"
#include "pin_mapping.h"
#include "i2c.h"
#include "led.h"
#include "ipmb.h"
#include "ipmi.h"
#include "sdr.h"
#include "payload.h"
#include "board_version.h"

#define DEBUG_LED
#define DEBUG_SDR

/* LED pins initialization */
static void prvHardwareInit( void );
TickType_t getTickDifference(TickType_t current_time, TickType_t start_time);
void heap_test ( void* param);
/*-----------------------------------------------------------*/

int main(void)
{

    /* Update clock register value - LPC specific */
    SystemCoreClockUpdate();

    /* Configure LED pins */
    prvHardwareInit();

//#define STOP_TEST
#ifdef STOP_TEST
    int test = 0;
    while (test == 0)
    {}
#endif

    LED_init();

    afc_board_i2c_init();
    afc_board_discover();

    /*  Init IPMI interface */
    /* NOTE: ipmb_init() is called inside this function */
    ipmi_init();

    sdr_init(ipmb_addr);
    payload_init();
    do_quiesced_init();
    sensor_init();

#ifdef HEAP_TEST
    xTaskCreate( heap_test, "Heap Test", 50, (void *) NULL, tskIDLE_PRIORITY+1, (TaskHandle_t *) NULL );
#endif
/* Start the tasks running. */
    vTaskStartScheduler();

    /* If all is well we will never reach here as the scheduler will now be
       running.  If we do reach here then it is likely that there was insufficient
       heap available for the idle task to be created. */
    for( ;; );

}
/*-----------------------------------------------------------*/

TickType_t getTickDifference(TickType_t current_time, TickType_t start_time)
{
    TickType_t result = 0;
    if (current_time < start_time) {
        result = start_time - current_time;
        result = portMAX_DELAY - result;
    } else {
        result = current_time - start_time;
    }
    return result;
}

#ifdef HEAP_TEST
void heap_test ( void* param)
{
    uint8_t water_mark = 0;
    size_t used_heap = 0;
    for (;;) {
	vTaskDelay(1000);
	water_mark = uxTaskGetStackHighWaterMark(NULL);
	used_heap = configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize();
    }
}
#endif

void prvToggleLED( LED_id led )
{
    unsigned long ulLEDState;
    unsigned long ulLEDport;
    unsigned long ulLEDpin;

    switch( led ){
    case LED_BLUE:
        ulLEDport = LEDBLUE_PORT;
        ulLEDpin = LEDBLUE_PIN;
        break;

    case LED_GREEN:
        ulLEDport = LEDGREEN_PORT;
        ulLEDpin = LEDGREEN_PIN;
        break;

    case LED_RED:
        ulLEDport = LEDRED_PORT;
        ulLEDpin = LEDRED_PIN;
        break;
    }
    /* Obtain the current P0 state. */
    ulLEDState = Chip_GPIO_GetPinState(LPC_GPIO, ulLEDport, ulLEDpin);

    /* Turn the LED off if it was on, and on if it was off. */
    Chip_GPIO_SetPinState(LPC_GPIO, ulLEDport, ulLEDpin, !ulLEDState);
}
/*-----------------------------------------------------------*/

static void prvHardwareInit ( void )
{
    /* Init LED Pin */
    Chip_GPIO_Init(LPC_GPIO);
    /* Set pin as output */
    Chip_GPIO_SetPinDIR(LPC_GPIO, LEDBLUE_PORT, LEDBLUE_PIN, true);
    Chip_GPIO_SetPinDIR(LPC_GPIO, LEDGREEN_PORT, LEDGREEN_PIN, true);
    Chip_GPIO_SetPinDIR(LPC_GPIO, LEDRED_PORT, LEDRED_PIN, true);
    /* Init GAddr test pin as output */
    Chip_GPIO_SetPinDIR(LPC_GPIO, GA_TEST_PORT, GA_TEST_PIN, true);
}
/*-----------------------------------------------------------*/
/* FreeRTOS Debug Functions */

#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook ( TaskHandle_t pxTask, signed char * pcTaskName){
    (void) pxTask;
    (void) pcTaskName;
    taskDISABLE_INTERRUPTS();
    /* Place a breakpoint here, so we know when there's a stack overflow */
    for ( ; ; ) {}
}
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
void vConfigureTimerForRunTimeStats( void )
{
    const unsigned long CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

    /* Power up and feed the timer with a clock. */
    Chip_TIMER_Init(LPC_TIMER0);
    /* Reset Timer 0 */
    Chip_TIMER_Reset(LPC_TIMER0);
    /* Just count up. */
    LPC_TIMER0->CTCR = CTCR_CTM_TIMER;

    /* Prescale to a frequency that is good enough to get a decent resolution,
       but not too fast so as to overflow all the time. */
    LPC_TIMER0->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

    /* Start the counter. */
    LPC_TIMER0->TCR = TCR_COUNT_ENABLE;
}
#endif

void vAssertCalled( char* file, uint32_t line) {
    taskDISABLE_INTERRUPTS();
    prvToggleLED(LED_RED);
    for( ;; );
}

#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void ) {
    for( ;; );
}
#endif

