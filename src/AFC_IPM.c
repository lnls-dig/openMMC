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
#include "board_defs.h"
#include "i2c.h"
#include "led.h"

/* Priorities at which the tasks are created. */
#define mainI2C0_TASK_PRIORITY		        ( tskIDLE_PRIORITY + 3 )
#define mainI2C1_TASK_PRIORITY              ( tskIDLE_PRIORITY + 2 )
#define mainTEST_TASK_PRIORITY              ( tskIDLE_PRIORITY + 1 )

/* LM75 Addresses */
#define mainLM75_1_ADDR                     ((uint32_t) 0x4C)
#define mainLM75_2_ADDR                     ((uint32_t) 0x4D)
#define mainLM75_3_ADDR                     ((uint32_t) 0x4E)
#define mainLM75_4_ADDR                     ((uint32_t) 0x4F)
#define mainREAD_TEMP_FREQUENCY_MS          200

/* Debug flags
 * -> I2C0 = 0
 * -> I2C1 = 1
 * -> Both = 3
 */
#define DEBUG_I2C   3

/* Tasks function prototypes */
static void prvMasterTestTask( void *pvParameters );
static void prvSlaveTestTask( void *pvParameters );

/* Function to toggle the LED */
static void prvToggleLED( LED_id led );
/* LED pins initialization */
static void prvHardwareInit( void );

/*-----------------------------------------------------------*/

int main(void)
{
    /* Configure LED pins */
    prvHardwareInit();

    /* Create project's tasks */
#if ((DEBUG_I2C == 0) || (DEBUG_I2C == 3))
    vI2CInit(I2C0, I2C_Mode_IPMB);
    xTaskCreate( prvSlaveTestTask, (const char*)"Slave Test", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, (UBaseType_t) 2, ( TaskHandle_t * ) NULL );

#endif
#if ((DEBUG_I2C == 1) || (DEBUG_I2C == 3))
    vI2CInit(I2C1, I2C_Mode_Local_Master);
    xTaskCreate( prvMasterTestTask, (const char*)"Master Test", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, (UBaseType_t) 1, ( TaskHandle_t * ) NULL );

#endif
    /* Start the tasks running. */
    vTaskStartScheduler();

    /* If all is well we will never reach here as the scheduler will now be
       running.  If we do reach here then it is likely that there was insufficient
       heap available for the idle task to be created. */
    for( ;; );
}
/*-----------------------------------------------------------*/
#if ((DEBUG_I2C == 1) || (DEBUG_I2C == 3))
static void prvMasterTestTask( void *pvParameters )
{
    uint8_t rx_data[i2cMAX_MSG_LENGTH];
#if 0
    /* Variable that stores the actual stack used by this task */
    uint8_t stack = uxTaskGetStackHighWaterMark( xTaskGetCurrentTaskHandle() );
#endif
    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
           The block state is specified in ticks, the constant used converts ticks
           to ms.  While in the blocked state this task will not consume any CPU
           time. */
        vTaskDelay( 50 / portTICK_PERIOD_MS );

        /* Send I2C message to the queue  */
        xI2CRead( I2C1, mainLM75_1_ADDR, rx_data, 2 );

        if (*rx_data < 40){
            prvToggleLED( LED_GREEN );
        }
    }
}

/*-----------------------------------------------------------*/
#endif
#if ((DEBUG_I2C == 0) || (DEBUG_I2C == 3))

static void prvSlaveTestTask( void *pvParameters )
{
    uint8_t rx_buff[i2cMAX_MSG_LENGTH];
    static uint8_t mch_retries;
    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
           The block state is specified in ticks, the constant used converts ticks
           to ms.  While in the blocked state this task will not consume any CPU
           time. */
        xI2CSlaveTransfer( I2C0, rx_buff, portMAX_DELAY );
        if (rx_buff[2] == 32){
            mch_retries++;
            if ( mch_retries == 3 ){
                prvToggleLED( LED_RED );
                mch_retries = 0;
            }
        }
    }
}
#endif
/*-----------------------------------------------------------*/

static void prvToggleLED( LED_id led )
{
    unsigned long ulLEDState;
    unsigned long ulLEDport;
    unsigned long ulLEDpin;

    switch( led ){
        case LED_BLUE:
            ulLEDport = ledBLUE_PORT;
            ulLEDpin = ledBLUE_PIN;
            break;

        case LED_GREEN:
            ulLEDport = ledGREEN_PORT;
            ulLEDpin = ledGREEN_PIN;
            break;

        case LED_RED:
            ulLEDport = ledRED_PORT;
            ulLEDpin = ledRED_PIN;
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
    /* Update clock register value */
    SystemCoreClockUpdate();

    /* Init LED Pin */
    Chip_GPIO_Init(LPC_GPIO);
    /* Set pin as output */
    Chip_GPIO_SetPinDIR(LPC_GPIO, ledBLUE_PORT, ledBLUE_PIN, true);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ledGREEN_PORT, ledGREEN_PIN, true);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ledRED_PORT, ledRED_PIN, true);
    /* Init GAddr test pin as output */
    Chip_GPIO_SetPinDIR(LPC_GPIO, GA_PORT, GA_TEST_PIN, true);
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
