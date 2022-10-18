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
 * @defgroup STM32F3_ADC STM32F3_ADC - STM32F3's ADC
 * @ingroup SENSORS
 *
 * The STM32F3_ADC is an ADC on STM32F3.
 * The host can query the STM32F3_ADC at any time to read digitalized values. <br>
 */

/**
 * @file stm32f3_adc.h
 * @author Adam Wujek  <adam.wujek@cern.ch>, CERN
 *
 * @brief Definitions for STM32F3_ADC.
 *
 * @ingroup STM32F3_ADC
 */

#ifndef STM32F3_ADC_H_
#define STM32F3_ADC_H_

#include <stm32f30x.h>
/**
 * @brief Rate at which the STM32F3_ADC sensors are read (in ms)
 */
#define STM32F3_ADC_UPDATE_RATE        1000

extern TaskHandle_t vTaskSTM32F3_ADC_Handle;

struct stm32f3_adc_def {
    ADC_TypeDef* ADCx;
    uint32_t RCC_PLLCLK;
    uint32_t RCC_AHBPeriph;
    ADC_CommonInitTypeDef *ADC_CommonInitStructure;
    ADC_InitTypeDef *ADC_InitStructure;
};

struct stm32f3_adc_ch_def {
    ADC_TypeDef* ADCx;
    uint8_t channel;
};


/**
 * @brief Initializes STM32F3_ADC monitoring task
 *
 * @return None
 */
void STM32F3_ADC_init( void );

/**
 * @brief Monitoring task for STM32F3_ADC sensor
 *
 * This task unblocks after every #STM32F3_ADC_UPDATE_RATE ms and updates the read from all the STM32F3_ADC sensors listed in this module's SDR table
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskSTM32F3_ADC( void* Parameters );

#endif
