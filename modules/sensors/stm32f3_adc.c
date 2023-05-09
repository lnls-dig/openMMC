/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2019 CERN
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

/**
 * @file stm32f3_adc.c
 * @author Adam Wujek  <adam.wujek@cern.ch>, CERN
 *
 * @brief Interface functions for ADC on stm32f3
 *
 * @ingroup STM32F3_ADC
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "port.h"

/* Project Includes */
#include "sdr.h"
#include "task_priorities.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "stm32f3_adc.h"
#include "board_stm32f3_adc.h"
#include "utils.h"
#include "uart_debug.h"

TaskHandle_t vTaskSTM32F3_ADC_Handle;

static void ADC_ForceDisable(ADC_TypeDef *theAdc)
{
  while (theAdc->CR & ADC_CR_ADEN)
  {
    theAdc->CR |= ADC_CR_ADDIS;
  }
}

static void init_adc()
{
    int adc_i;
    volatile int calibration_value[STM32F3_ADC_ID_MAX_CNT];

    /* To avaoid compiler warning */
    (void)calibration_value;

    for (adc_i = 0; adc_i < STM32F3_ADC_ID_MAX_CNT; adc_i++) {
	ADC_ForceDisable(stm32f3_adc_config[adc_i].ADCx);
	ADC_DeInit(stm32f3_adc_config[adc_i].ADCx);
    }

    for (adc_i = 0; adc_i < STM32F3_ADC_ID_MAX_CNT; adc_i++) {
	/* Configure the ADC clock */
	RCC_ADCCLKConfig(stm32f3_adc_config[adc_i].RCC_PLLCLK);

	/* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(stm32f3_adc_config[adc_i].RCC_AHBPeriph, ENABLE);
    }

    for (adc_i = 0; adc_i < STM32F3_ADC_ID_MAX_CNT; adc_i++) {
	ADC_CommonInit(stm32f3_adc_config[adc_i].ADCx, stm32f3_adc_config[adc_i].ADC_CommonInitStructure);
	ADC_Init(stm32f3_adc_config[adc_i].ADCx, stm32f3_adc_config[adc_i].ADC_InitStructure);
	ADC_VoltageRegulatorCmd(stm32f3_adc_config[adc_i].ADCx, ENABLE);
    }

    /* Insert delay equal to 10 us */
    vTaskDelay(10);


    /* Calibration procedure */
    for (adc_i = 0; adc_i < STM32F3_ADC_ID_MAX_CNT; adc_i++) {
	int i = 10000;
	ADC_SelectCalibrationMode(stm32f3_adc_config[adc_i].ADCx, ADC_CalibrationMode_Single);
	ADC_StartCalibration(stm32f3_adc_config[adc_i].ADCx);

	while (ADC_GetCalibrationStatus(stm32f3_adc_config[adc_i].ADCx) != RESET && (i > 0)) {
	    i--;
	    printf(".");
	    asm volatile("nop");
	}
	calibration_value[adc_i]= ADC_GetCalibrationValue(stm32f3_adc_config[adc_i].ADCx);
	ADC_Cmd(stm32f3_adc_config[adc_i].ADCx, ENABLE);
	while (!ADC_GetFlagStatus(stm32f3_adc_config[adc_i].ADCx, ADC_FLAG_RDY))
	    ;
    }
}


static void adc_start_conversion(ADC_TypeDef* ADCx, int channel)
{
    /* stop conversion before changing the cannel */
    ADC_StopConversion(ADCx);
    while (ADC_GetStartConversionStatus(ADCx) != RESET){ printf("x");}
    ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_7Cycles5);

    /* Start ADC1 Software Conversion */
    ADC_StartConversion(ADCx);
}

static uint16_t adc_read(ADC_TypeDef* ADCx)
{
  uint16_t val = ADC_GetConversionValue(ADCx);
  return val;
}

static uint16_t adc_get_value(ADC_TypeDef* ADCx, int channel)
{
    adc_start_conversion(ADCx, channel);
    while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET){
	printf(".");
    }
    vTaskDelay(1); /* needed */
    int rv = adc_read(ADCx);
    //printf("ch %d rv %d\n\r", channel, rv );
    return rv;
}

void vTaskSTM32F3_ADC( void* Parameters )
{
    const TickType_t xFrequency = STM32F3_ADC_UPDATE_RATE;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    sensor_t * adc_sensor;
    uint16_t adc_val;

    /* init adc */
    init_adc();

    /* board specific init */
    board_stm32f3_adc_init();

    vTaskDelay(1000);
    xLastWakeTime = xTaskGetTickCount();
    for ( ;; ) {
        vTaskDelay( portTICK_RATE_MS * 100 );

        /* Iterate through the SDR Table to find all the STM32F3_ADC entries */

        for ( adc_sensor = sdr_head; adc_sensor != NULL; adc_sensor = adc_sensor->next) {

            if ( adc_sensor->task_handle == NULL ) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(adc_sensor->task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            adc_val = adc_get_value(stm32f3_adc_ch_config[adc_sensor->chipid].ADCx,
                                    stm32f3_adc_ch_config[adc_sensor->chipid].channel);
            adc_sensor->readout_value = adc_val >> 4;
        }

        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}


void STM32F3_ADC_init( void )
{
    xTaskCreate( vTaskSTM32F3_ADC, "x", 200, (void *) NULL, tskIDLE_PRIORITY+1, &vTaskSTM32F3_ADC_Handle);
}
