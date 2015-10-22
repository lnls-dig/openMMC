/*
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

/*!
 * @file ina220.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief DCDCs Voltage reading using INA220 sensor
 */

#include "FreeRTOS.h"
#include "task.h"
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "board_version.h"
#include "payload.h"
#include "ina220.h"

// INA220 7bit address
// 0x40 - FMC2 P12V
// 0x41 - FMC2 PVADJ
// 0x43 - FMC2 P3V3

// 0x45 - FMC1 P12V
// 0x42 - FMC1 PVADJ
// 0x44 - FMC1 P3V3

void vTaskINA220( void * Parameters )
{
    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = INA220_UPDATE_RATE / portTICK_PERIOD_MS;

    uint8_t i2c_bus_id;
    uint8_t i;
    SDR_type_01h_t *pSDR;
    sensor_data_entry_t * pDATA;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for ( ;; ) {

        /* Update all [voltage sensors readings */
        for ( i = 0; i < NUM_SDR; i++ ) {

            /* Check if the handle pointer is not NULL */
            if (sensor_array[i].task_handle == NULL) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(sensor_array[i].task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            pSDR = (SDR_type_01h_t *) sensor_array[i].sdr;
            pDATA = sensor_array[i].data;

            /* Try to gain the I2C bus */
            if (afc_i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t)100) == pdFALSE) {
                continue;
            }

	    /* Update sensor reading on SDR */
	    pDATA->readout_value = INA220_readVolt( i2c_bus_id, sensor_array[i].slave_addr, true );

	    /* Take different action for each sensor */
	    switch ((pSDR->hdr.recID_MSB << 8)|(pSDR->hdr.recID_LSB)) {
	    case NUM_SDR_FMC2_12V:
		/* Compare reading with sensor threshold levels */
	        if (pDATA->readout_value > pSDR->lower_noncritical_thr) {
		    payload_send_message(PAYLOAD_MESSAGE_P12GOOD);
		} else if (pDATA->readout_value < pSDR->lower_critical_thr) {
		    payload_send_message(PAYLOAD_MESSAGE_P12GOODn);
		}
	        break;
	    default:
	        break;
	    }




	    afc_i2c_give( i2c_bus_id );
        }
	vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

void INA220_init( void )
{
#define INA_CFG
    uint8_t i, j;
    uint8_t i2c_bus_id;

    xTaskCreate( vTaskINA220, "INA220", configMINIMAL_STACK_SIZE, (void *) NULL, tskINA220SENSOR_PRIORITY, &vTaskINA220_Handle);
#ifdef INA_CFG

    for ( j = 0, i = 0; i < NUM_SDR && j < MAX_INA220_COUNT; i++, j++ ) {
        if ( *(sensor_array[i].task_handle) != vTaskINA220_Handle ) {
            continue;
        }

        if (afc_i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t) 50) == pdFALSE) {
            continue;
        }

        uint8_t cmd[3] = {0x00, 0x01, 0x9F};

        portENABLE_INTERRUPTS();
        xI2CMasterWrite(i2c_bus_id, sensor_array[i].slave_addr, cmd, sizeof(cmd));
        portDISABLE_INTERRUPTS();

        afc_i2c_give( i2c_bus_id );
    }

#endif

}

uint16_t INA220_readVolt(I2C_ID_T i2c, uint8_t address, bool raw)
{
    uint8_t ch[2];
    uint16_t tmpVal = 0;

    xI2CMasterWriteRead(i2c, address, INA220_BUS_REG, ch, 2);
    tmpVal = (0x1fE0 & (ch[0] << 5)) | (0x1f & (ch[1] >> 3));
    if (raw == false) {
        tmpVal = tmpVal * 4;
    }
    return tmpVal/16;
}
