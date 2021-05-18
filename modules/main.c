/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/* Kernel includes. */
#include "FreeRTOS.h"

/* Project includes */
#include "GitSHA1.h"
#include "port.h"
#include "led.h"
#include "pin_cfg.h"
#include "ipmi.h"
#ifdef MODULE_PAYLOAD
#include "payload.h"
#endif
#include "i2c.h"
#include "fru.h"
#include "scansta1101.h"
#include "fpga_spi.h"
#include "watchdog.h"
#include "uart_debug.h"

#ifdef MODULE_CLI
#include "cli.h"
#endif

#ifdef MODULE_RTM
#include "rtm.h"
#endif

#ifdef MODULE_BOARD_CONFIG
#include "board_config.h"
#endif

/*-----------------------------------------------------------*/
int main( void )
{
    pin_init();

#ifdef MODULE_BOARD_CONFIG
    board_init();
#endif

#ifdef MODULE_UART_DEBUG
    uart_init( UART_DEBUG );
#endif

    printf("openMMC Starting!\n");
    printf("Build date: %s %s\n", __DATE__, __TIME__);
    printf("Version: %s\n", g_GIT_TAG);
    printf("SHA1: %s\n", g_GIT_SHA1);

#ifdef MODULE_WATCHDOG
    watchdog_init();
#endif

    LED_init();
    i2c_init();

    ipmb_addr = get_ipmb_addr( );

    if (bench_test) {
        printf("BENCH_TEST mode activated! This will enable some debug functions, be careful!\n");
    }

#ifdef MODULE_FRU
    fru_init(FRU_AMC);
#endif

#ifdef MODULE_SDR
    sdr_init();
#endif
#ifdef MODULE_SENSORS
    sensor_init();
#endif
#ifdef MODULE_PAYLOAD
    payload_init();
#endif
#ifdef MODULE_SCANSTA1101
    scansta1101_init();
#endif
#ifdef MODULE_FPGA_SPI
    fpga_spi_init();
#endif
#ifdef MODULE_RTM
    rtm_manage_init();
#endif

#ifdef MODULE_CLI
    cli_init();
#endif
    /*  Init IPMI interface */
    /* NOTE: ipmb_init() is called inside this function */
    ipmi_init();

    /* Start the tasks running. */
    vTaskStartScheduler();

    /* If all is well we will never reach here as the scheduler will now be
       running.  If we do reach here then it is likely that there was insufficient
       heap available for the idle task to be created. */
    for( ;; );

}


/* FreeRTOS debug functions */
#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void ) {}
#endif

#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook ( TaskHandle_t pxTask, signed char * pcTaskName)
{
    (void) pxTask;
    (void) pcTaskName;
    taskDISABLE_INTERRUPTS();
    /* Place a breakpoint here, so we know when there's a stack overflow */
    for ( ; ; ) {
        uxTaskGetStackHighWaterMark(pxTask);
    }
}
#endif

