/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
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

#include "FreeRTOS.h"
#include "task.h"
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "board_version.h"
#include "lm75.h"

TaskHandle_t vTaskLM75_Handle;

void vTaskLM75( void* Parameters )
{
    const TickType_t xFrequency = LM75_UPDATE_RATE;

    uint8_t i2c_bus_id;
    uint8_t i;

    sensor_t * lm75_sensor;
    uint8_t temp[2];
    uint16_t converted_temp;

    for ( ;; ) {
        /* Try to gain the I2C bus */
        if (i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t)100) == pdTRUE) {
            /* Update all temperature sensors readings */
            for ( i = 0; i < NUM_SDR; i++ ) {
                /* Check if the handle pointer is not NULL */
                if (sensor_array[i].task_handle == NULL) {
                    continue;
                }
                /* Check if this task should update the selected SDR */
                if ( *(sensor_array[i].task_handle) != xTaskGetCurrentTaskHandle() ) {
                    continue;
                }

		lm75_sensor = &sensor_array[i];

                /* Update the temperature reading */
                if (xI2CMasterRead( i2c_bus_id, sensor_array[i].slave_addr, &temp[0], 2) == 2) {
                    converted_temp = ((temp[0] << 1) | ((temp[1]>>8)));
                    lm75_sensor->readout_value = converted_temp;
                }
            }
            i2c_give(i2c_bus_id);
        }
        vTaskDelay(xFrequency);
    }
}

void LM75_init( void )
{
#ifdef LM75_CFG
    uint8_t i2c_bus_id;
    uint8_t i, j;

    for ( j = 0, i = 0; i < NUM_SDR && j < LM75_MAX_COUNT; i++, j++ ) {
        if (i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t) 50) == pdFALSE) {
            continue;
        }
        uint8_t cmd[3] = {0x00, 0x01, 0x9F};
        xI2CMasterWrite(i2c_id, sensor_array[i].slave_addr, ch, 3);

        i2c_give( i2c_bus_id );
    }
#endif
    xTaskCreate( vTaskLM75, "LM75", 330, (void *) NULL, tskLM75SENSOR_PRIORITY, &vTaskLM75_Handle);
}

/* LM75 SDR List */

const SDR_type_01h_t SDR_LM75_uC = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x01, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 5, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (30 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (20 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (75 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (65 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (55 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (5 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (10 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (20 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP UC"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP UC" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_CLOCK_SWITCH = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x01, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 5, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (30 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (20 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (75 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (65 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (55 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (5 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (10 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (20 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP CLK SWITCH"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP CLK SWITCH" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_DCDC = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x01, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 5, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (30 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (20 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (75 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (65 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (55 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (5 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (10 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (20 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP DCDC"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP DCDC" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_RAM = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x01, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 5, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (30 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (20 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (75 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (65 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (55 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (5 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (10 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (20 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP RAM"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP RAM" /* sensor string */
};
