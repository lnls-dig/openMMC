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

#ifdef OLD_INA220
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

#else

const t_ina220_config ina220_cfg = {
    .config_default = INA220_CONFIG_DEFAULT,
    .calibration_factor = 40960000,
    .registers = INA220_REGISTERS,
    .shunt_div = 100,
    .bus_voltage_shift = 3,
    .bus_voltage_lsb = 4000,
    .power_lsb = 20000,
};

static t_ina220_data ina220_data[MAX_INA220_COUNT];


void vTaskINA220( void *Parameters )
{
    uint8_t i;

    TickType_t xLastWakeTime;
    /* Task will run every 100ms */
    const TickType_t xFrequency = INA220_UPDATE_RATE / portTICK_PERIOD_MS;

    SDR_type_01h_t *pSDR;
    sensor_data_entry_t * pDATA;
    t_ina220_data * data_ptr;

    /* Initialise the xLastWakeTime variable with the current time. */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Read all registers from the INA220s */
        for ( i = 0; i < MAX_INA220_COUNT; i++) {
	    ina220_readall( &ina220_data[i] );
	}

        for ( i = 0; i < NUM_SDR; i++ ) {
            /* Update their SDR */
            /* Check if the handle pointer is not NULL */
            if (sensor_array[i].task_handle == NULL) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(sensor_array[i].task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

	    pSDR = sensor_array[i].sdr;
            pDATA = sensor_array[i].data;
	    data_ptr = pDATA->sensor_info;

            switch (pSDR->sensortype) {
            case SENSOR_TYPE_VOLTAGE:
                pDATA->readout_value = ((data_ptr->regs[INA220_BUS_VOLTAGE] >> data_ptr->config->bus_voltage_shift) * data_ptr->config->bus_voltage_lsb) / 100000;
		break;
	    case SENSOR_TYPE_CURRENT:
		/* Current in mA */
		pDATA->readout_value = data_ptr->regs[INA220_CURRENT];
		break;
	    default:
		/* Shunt voltage and power not implemented */
		break;
	    }

	    /* Maybe implement a callback function for each sensor type, and assign on initialization, in a more generic way for all sensors */
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

	    vTaskDelayUntil( &xLastWakeTime, xFrequency );
        }
    }
}

uint8_t ina220_config(uint8_t i2c_id, t_ina220_data * data)
{
    uint8_t i2c_bus = I2C1;

    data->config = &ina220_cfg;
    data->i2c_id = i2c_id;
    if ((data->rshunt == 0) || (data->rshunt > data->config->calibration_factor)) {
        data->rshunt = INA220_RSHUNT_DEFAULT;
    } else {
        return -1;
    }
    data->curr_config = data->config->config_default;

    /* Change to 'afc_i2c_take_by_chipid', which is more generic */
    if( afc_i2c_take_by_busid( I2C_BUS_CPU_ID, &(i2c_bus), (TickType_t) 100) == pdFALSE ) {
        return -1;
    }

    uint8_t cfg_buff[3] = { INA220_CONFIG, ( data->curr_config >> 8 ), ( data->curr_config & 0xFFFF ) };

    portENABLE_INTERRUPTS();
    xI2CMasterWrite( i2c_bus, data->i2c_id, cfg_buff, sizeof(cfg_buff)/sizeof(cfg_buff[0]) );
    portDISABLE_INTERRUPTS();

    afc_i2c_give(i2c_bus);

    return 0;
}

uint16_t ina220_readvalue( t_ina220_data * data, uint8_t reg )
{
    uint8_t i2c_bus;
    uint8_t val[2];

    /*! @todo: Change to 'afc_i2c_take_by_chipid', which is more generic */
    if( afc_i2c_take_by_busid( I2C_BUS_CPU_ID, &(i2c_bus), (TickType_t) 10) == pdFALSE ) {
        return -1;
    }

    xI2CMasterWriteRead( i2c_bus, data->i2c_id, reg, &val[0], sizeof(val)/sizeof(val[0]) );

    afc_i2c_give( i2c_bus );

    return ( (val[0] << 8) | (val[1]) );
}

void ina220_readall( t_ina220_data * data )
{
    /* Read all INA220 Registers */
    for ( uint8_t i = 0; i < INA220_REGISTERS; i++ ) {
        data->regs[i] = ina220_readvalue( data, i );
    }
}


void ina220_sdr_init ( TaskHandle_t handle )
{
    uint8_t i, j;
    t_ina220_data * data_ptr;

    while (i < MAX_INA220_COUNT) {
        for ( j = 0; j < NUM_SDR; j++ ) {
            /* Update their SDR */
            /* Check if the handle pointer is not NULL */
            if (sensor_array[j].task_handle == NULL) {
                continue;
            }

            /* Check if this task should update the selected SDR */
            if ( *(sensor_array[j].task_handle) != handle ) {
                continue;
            }

            sensor_array[j].data->sensor_info = &ina220_data[i];
            data_ptr = (t_ina220_data *) sensor_array[j].data->sensor_info;
            data_ptr->i2c_id = sensor_array[j].slave_addr;
            ina220_config( data_ptr->i2c_id, data_ptr );
            i++;
        }
    }

}

void ina220_init( void )
{
    xTaskCreate( vTaskINA220, "INA220", 400, (void *) NULL, tskINA220SENSOR_PRIORITY, &vTaskINA220_Handle);

    configASSERT(vTaskINA220_Handle);

    ina220_sdr_init(vTaskINA220_Handle);
}

#endif
