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
#include "ipmi.h"
#include "sdr.h"
#include "payload.h"
#include "board_version.h"
#include "fru.h"
#include "jtag.h"
#include "fpga_spi.h"
#include "watchdog.h"

//#define HEAP_TEST
//#define STOP_TEST

/* LED pins initialization */
void heap_test ( void* param);
TaskHandle_t heap_handle;
/*-----------------------------------------------------------*/

int main(void)
{
    /* Update clock register value - LPC specific */
    SystemCoreClockUpdate();

#if (configGENERATE_RUN_TIME_STATS == 1)
    vConfigureTimerForRunTimeStats();
#endif

#ifdef STOP_TEST
    int test = 0;
    while (test == 0)
    {}
#endif
#ifdef MODULE_WATCHDOG
    watchdog_init();
#endif

    LED_init();
#ifdef MODULE_FRU
    fru_init();
#endif

    portENABLE_INTERRUPTS();
    afc_board_i2c_init();
    afc_board_discover();
    portDISABLE_INTERRUPTS();

    ipmb_addr = get_ipmb_addr();
#ifdef MODULE_SDR
    sdr_init(ipmb_addr);
#endif
#ifdef MODULE_SENSORS
    sensor_init();
#endif
#ifdef MODULE_PAYLOAD
    payload_init();
#endif
#ifdef MODULE_JTAG_SWITCH
    init_scansta();
#endif
#ifdef MODULE_FPGA_SPI
    init_fpga_spi();
#endif
    /*  Init IPMI interface */
    /* NOTE: ipmb_init() is called inside this function */
    ipmi_init();
#ifdef HEAP_TEST
    xTaskCreate( heap_test, "Heap Test", 50, (void *) NULL, tskIDLE_PRIORITY+5, &heap_handle );
#endif
/* Start the tasks running. */
    vTaskStartScheduler();

    /* If all is well we will never reach here as the scheduler will now be
       running.  If we do reach here then it is likely that there was insufficient
       heap available for the idle task to be created. */
    for( ;; );

}
/*-----------------------------------------------------------*/
#ifdef HEAP_TEST
static char stats[500];
void heap_test ( void* param)
{
    uint8_t water_mark = 0;
    size_t used_heap = 0;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	water_mark = uxTaskGetStackHighWaterMark(NULL);
	used_heap = configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize();
	vTaskGetRunTimeStats(stats);
    }
}
#endif

/*-----------------------------------------------------------*/
/* FreeRTOS Debug Functions */

#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook ( TaskHandle_t pxTask, signed char * pcTaskName){
    (void) pxTask;
    (void) pcTaskName;
    taskDISABLE_INTERRUPTS();
    /* Place a breakpoint here, so we know when there's a stack overflow */
    for ( ; ; ) {
        uxTaskGetStackHighWaterMark(pxTask);
    }
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
    for( ;; );
}

#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void ) {

}
#endif

void vApplicationIdleHook (void) {

}
