/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) Vahan Petrosyan <vahan_petrosyan@desy.de>
 *   Copyright (C) Frederic Bompard (CPPM)
 *   Copyright (C) Paschalis Vichoudis (CERN)
 *   Copyright (C) Markus Joos <markus.joos@cern.ch>
 *   Copyright (C) Julian Mendez <julian.mendez@cern.ch>
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Project Includes */
#include "sdr.h"
#include "sensors.h"
#include "ipmi.h"
#include "fpga_spi.h"

sensor_t *sensor_array = NULL;
volatile uint8_t sdr_count = 0;

static uint16_t reservationID;

size_t sdr_get_size_by_type(SDR_TYPE type)
{
    switch (type) {
    case TYPE_01:
        return sizeof(SDR_type_01h_t);
    case TYPE_02:
        return sizeof(SDR_type_02h_t);
    case TYPE_12:
        return sizeof(SDR_type_12h_t);
    default:
        return 0;
    }
}

size_t sdr_get_size_by_entry(uint8_t id)
{
    if (id >= sdr_count) {
        return 0;
    }
    return sdr_get_size_by_type(sensor_array[id].sdr_type);
}

void sensor_init( void )
{
    /* This function must be provided by the board port */
    user_sdr_init();

    hotswap_init();
    LM75_init();
    ina220_init();
}

void sdr_init( void )
{
    sensor_array = pvPortMalloc( sizeof(sensor_t) );

    configASSERT(sensor_array);

    /* Populate SDR Device Locator Record */
    sensor_array[0].num = 0;
    sensor_array[0].sdr_type = TYPE_12;
    sensor_array[0].sdr = (void *) &SDR0;
    sensor_array[0].sdr_length = sizeof(SDR0);
    sensor_array[0].task_handle = NULL;
    sensor_array[0].diag_devID = NO_DIAG;
    sensor_array[0].slave_addr = ipmb_addr;
    sensor_array[0].entityinstance =  0x60 | ((ipmb_addr - 0x70) >> 1);

    sdr_count++;
}

/** @todo: Maybe use a dynamic linked table to hold the SDR entries, since we'll have to remove them later */

void sdr_insert_entry( SDR_TYPE type, void * sdr, TaskHandle_t *monitor_task, uint8_t diag_id, uint8_t slave_addr)
{
    uint8_t index = sdr_count;
    uint8_t sdr_len = sdr_get_size_by_type(type);

    sensor_array = pvPortRealloc( sensor_array, sizeof(sensor_t)*(sdr_count+1) );

    sensor_array[index].num = index;
    sensor_array[index].sdr_type = type;
    sensor_array[index].sdr = sdr;
    sensor_array[index].sdr_length = sdr_len;
    sensor_array[index].task_handle = monitor_task;
    sensor_array[index].diag_devID = diag_id;
    sensor_array[index].slave_addr = slave_addr;
    sensor_array[index].ownerID = ipmb_addr;
    sensor_array[index].entityinstance =  0x60 | ((ipmb_addr - 0x70) >> 1);
    sensor_array[index].readout_value = 0;
    sensor_array[index].state = SENSOR_STATE_LOW_NON_REC;

    sdr_count++;
}

/******************************/
/* IPMI SDR Commands handlers */
/******************************/

IPMI_HANDLER(ipmi_se_get_sdr_info, NETFN_SE, IPMI_GET_DEVICE_SDR_INFO_CMD, ipmi_msg *req, ipmi_msg *rsp) {
    int len = rsp->data_len;

    if (req->data_len == 0 || req->data[0] == 0) {
	/* Return number of sensors only */
        rsp->data[len++] = sdr_count-1;
    } else {
	/* Return number of SDR entries */
        rsp->data[len++] = sdr_count;
    }
    /* Static Sensor population and LUN 0 has sensors */
    rsp->data[len++] = 0x01; // if dynamic additional 4 bytes required (see Table 20-2 Get Device SDR INFO Command
    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

/*!
 * @brief Handler for "Set Event Receiver" command, as on IPMIv2 1.1
 * section 29.1.
 *
 * Set (or reset) the address to which IPMI events
 * will be sent. Also, disable sending events if command 0xFF is received.
 *
 * @param[in] req Incoming request to be handled and answered.
 *
 * @return void
 */
uint8_t event_receiver_addr = 0xFF;
uint8_t event_receiver_lun = 0x00;

IPMI_HANDLER(ipmi_se_set_event_reciever, NETFN_SE, IPMI_SET_EVENT_RECEIVER_CMD, ipmi_msg *req, ipmi_msg *rsp)
{
    /* Update the event receiver address (even if its 0xFF,
       the checking will be done in check_sensor_event function) */
    event_receiver_addr = req->data[0];
    event_receiver_lun = req->data[1];

    rsp->completion_code = IPMI_CC_OK;
    rsp->data_len = 0;
}

/*!
 * @brief Handler for "Get Event Receiver" command, as on IPMIv2 1.1
 * section 29.1.
 *
 * Returns the previously set event receiver address and LUN
 *
 * @param[in] req Incoming request to be handled and answered.
 *
 * @return void
 */
IPMI_HANDLER(ipmi_se_get_event_reciever, NETFN_SE, IPMI_GET_EVENT_RECEIVER_CMD, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = 0;
    rsp->data[len++] = event_receiver_addr;
    rsp->data[len++] = event_receiver_lun;

    rsp->completion_code = IPMI_CC_OK;
    rsp->data_len = len;
}

IPMI_HANDLER(ipmi_se_get_sdr, NETFN_SE, IPMI_GET_DEVICE_SDR_CMD, ipmi_msg *req,  ipmi_msg* rsp)
{
    uint16_t recv_reserv_id = req->data[0] | (req->data[1] << 8);
    uint16_t record_id = req->data[2] | (req->data[3] << 8);
    uint8_t offset = req->data[4];
    uint8_t size = req->data[5];
    uint8_t len = rsp->data_len = 0;

    /* Reservation ID check */
    if (reservationID != recv_reserv_id) {
        rsp->data_len = 0;
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        return;
    }

    if ( (record_id >= NUM_SDR)
         || ((size + offset) > sensor_array[record_id].sdr_length)) {
        rsp->completion_code = IPMI_CC_REQ_DATA_NOT_PRESENT;
        return;
    } else if (record_id == NUM_SDR - 1) {
        rsp->data[len++] = 0xFF;
        rsp->data[len++] = 0xFF;
    } else {
        rsp->data[len++] = (record_id + 1) & 0xFF; /* next record ID */
        rsp->data[len++] = (record_id + 1) >> 8; /* next record ID */
    }

    uint8_t tmp_c, index;
    uint8_t * pSDR = (uint8_t*) sensor_array[record_id].sdr;
    uint8_t sdr_type = pSDR[3];

    for (uint8_t i = 0; i < size; i++) {
        index = i + offset;
        tmp_c = pSDR[index];

	/* Return not-const data from SDR */
	if (index == 0) {
	    tmp_c = record_id;
	} else if (index == 5) {
            tmp_c = sensor_array[record_id].ownerID;
        } else if ( sdr_type == TYPE_01 || sdr_type == TYPE_02 ) {
            if (index == 9) {
                tmp_c = sensor_array[record_id].entityinstance;
            }
	    if (index == 7) {
		tmp_c = record_id;
	    }
        } else if ( sdr_type == TYPE_11 || sdr_type == TYPE_12 ) {
            if (index == 13) {
                tmp_c = sensor_array[record_id].entityinstance;
            }
        }
        rsp->data[len++] = tmp_c;
    }

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_se_reserve_device_sdr, NETFN_SE, IPMI_RESERVE_DEVICE_SDR_REPOSITORY_CMD, ipmi_msg *req, ipmi_msg* rsp) {
    int len = rsp->data_len;

    reservationID++;
    if (reservationID == 0) {
        reservationID = 1;
    }
    rsp->data[len++] = reservationID & 0xff;
    rsp->data[len++] = reservationID >> 8;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_se_get_sensor_reading, NETFN_SE, IPMI_GET_SENSOR_READING_CMD,  ipmi_msg *req, ipmi_msg* rsp) {
    int sensor_number = req->data[0];
    int len = rsp->data_len;

    if (sensor_number > sdr_count) {
        rsp->completion_code = IPMI_CC_REQ_DATA_NOT_PRESENT;
        rsp->data_len = 0;
        return;
    }

    if (*(sensor_array[sensor_number].task_handle) == vTaskHotSwap_Handle) {
        rsp->data[len++] = 0x00;
        rsp->data[len++] = 0xC0;
        /* Current State Mask */
        rsp->data[len++] = sensor_array[sensor_number].readout_value;
    } else {
        rsp->data[len++] = sensor_array[sensor_number].readout_value;
        rsp->data[len++] = 0x40;
        /* Present threshold status */
        /* TODO: Implement threshold reading */
        rsp->data[len++] = 0xC0;
    }

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

/* Sensor alarm checking function adapted from CERN MMCv2 implementation, credits in this file header */
void check_sensor_event( sensor_t * sensor )
{
    /** Should be rewritten to be compliant with RTM management !! */
    uint8_t ev = 0xFF;
    uint8_t ev_type;

    configASSERT(sensor);

    SDR_type_01h_t * sdr = ( SDR_type_01h_t * ) sensor->sdr;
    configASSERT(sdr);

    if( sdr->hdr.rectype != TYPE_01 ) {
        return;
    }

    /** Compare value with threshold */
    switch(sensor->state) {
    case SENSOR_STATE_HIGH_NON_REC:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->upper_nonrecover_thr) - (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_recoverable_go_high){
                    ev = IPMI_THRESHOLD_UNR_GH;
                    sensor->asserted_event.upper_non_recoverable_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_HIGH_CRIT;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) <= ((int8_t)sdr->upper_nonrecover_thr) && !sensor->asserted_event.upper_non_recoverable_go_low){
                ev = IPMI_THRESHOLD_UNR_GL;
                sensor->asserted_event.upper_non_recoverable_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->upper_nonrecover_thr) + (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_recoverable_go_low){
                    ev = IPMI_THRESHOLD_UNR_GL;
                    sensor->asserted_event.upper_non_recoverable_go_low = 0;

                }
            }
        }
        else{
            if(sensor->readout_value <= (sdr->upper_nonrecover_thr - (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_recoverable_go_high){
                    ev = IPMI_THRESHOLD_UNR_GH;
                    sensor->asserted_event.upper_non_recoverable_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_HIGH_CRIT;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value <= sdr->upper_nonrecover_thr && !sensor->asserted_event.upper_non_recoverable_go_low){
                ev = IPMI_THRESHOLD_UNR_GL;
                sensor->asserted_event.upper_non_recoverable_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value >= (sdr->upper_nonrecover_thr + (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_recoverable_go_low){
                    ev = IPMI_THRESHOLD_UNR_GL;
                    sensor->asserted_event.upper_non_recoverable_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        break;

    case SENSOR_STATE_HIGH_CRIT:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) >= ((int8_t)sdr->upper_nonrecover_thr)){
                ev = IPMI_THRESHOLD_UNR_GH;
                sensor->asserted_event.upper_non_recoverable_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH_NON_REC;
            }
            else if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->upper_critical_thr) - (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_critical_go_high){
                    ev = IPMI_THRESHOLD_UC_GH;
                    sensor->asserted_event.upper_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_HIGH;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) <= ((int8_t)sdr->upper_critical_thr) && !sensor->asserted_event.upper_critical_go_low){
                ev = IPMI_THRESHOLD_UC_GL;
                sensor->asserted_event.upper_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->upper_critical_thr) + (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.upper_critical_go_low){
                    ev = IPMI_THRESHOLD_UC_GL;
                    sensor->asserted_event.upper_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        else{
            if(sensor->readout_value >= sdr->upper_nonrecover_thr){
                ev = IPMI_THRESHOLD_UNR_GH;
                sensor->asserted_event.upper_non_recoverable_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH_NON_REC;
            }
            else if(sensor->readout_value <= (sdr->upper_critical_thr - (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_critical_go_high){
                    ev = IPMI_THRESHOLD_UC_GH;
                    sensor->asserted_event.upper_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_HIGH;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value <= sdr->upper_critical_thr && !sensor->asserted_event.upper_critical_go_low){
                ev = IPMI_THRESHOLD_UC_GL;
                sensor->asserted_event.upper_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value >= (sdr->upper_critical_thr + (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.upper_critical_go_low){
                    ev = IPMI_THRESHOLD_UC_GL;
                    sensor->asserted_event.upper_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }

        break;

    case SENSOR_STATE_HIGH:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) >= ((int8_t)sdr->upper_critical_thr)){
                ev = IPMI_THRESHOLD_UC_GH;
                sensor->asserted_event.upper_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH_CRIT;
            }
            else if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->upper_noncritical_thr) - (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_critical_go_high){
                    ev = IPMI_THRESHOLD_UNC_GH;
                    sensor->asserted_event.upper_non_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_NORMAL;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) <= ((int8_t)sdr->upper_noncritical_thr) && !sensor->asserted_event.upper_non_critical_go_low){
                ev = IPMI_THRESHOLD_UNC_GL;
                sensor->asserted_event.upper_non_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->upper_noncritical_thr) + (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_critical_go_low){
                    ev = IPMI_THRESHOLD_UNC_GL;
                    sensor->asserted_event.upper_non_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        else{
            if(sensor->readout_value >= sdr->upper_critical_thr){
                ev = IPMI_THRESHOLD_UC_GH;
                sensor->asserted_event.upper_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH_CRIT;
            }
            else if(sensor->readout_value <= (sdr->upper_noncritical_thr - (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_critical_go_high){
                    ev = IPMI_THRESHOLD_UNC_GH;
                    sensor->asserted_event.upper_non_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_NORMAL;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value <= sdr->upper_noncritical_thr && !sensor->asserted_event.upper_non_critical_go_low){
                ev = IPMI_THRESHOLD_UNC_GL;
                sensor->asserted_event.upper_non_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value >= (sdr->upper_noncritical_thr + (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_critical_go_low){
                    ev = IPMI_THRESHOLD_UNC_GL;
                    sensor->asserted_event.upper_non_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        break;

    case SENSOR_STATE_NORMAL:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) >= ((int8_t)sdr->upper_noncritical_thr)){
                ev = IPMI_THRESHOLD_UNC_GH;
                sensor->asserted_event.upper_non_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH;
            }

            else if(((int8_t)sensor->readout_value) <= ((int8_t)sdr->lower_noncritical_thr)){
                ev = IPMI_THRESHOLD_LNC_GL;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW;
            }
        }
        else{
            if(sensor->readout_value >= sdr->upper_noncritical_thr){
                ev = IPMI_THRESHOLD_UNC_GH;
                sensor->asserted_event.upper_non_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_HIGH;
            }

            else if(sensor->readout_value <= sdr->lower_noncritical_thr){
                ev = IPMI_THRESHOLD_LNC_GL;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW;
            }
        }
        break;

    case SENSOR_STATE_LOW:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) <= ((int8_t)sdr->lower_critical_thr)){
                ev = IPMI_THRESHOLD_LC_GH;
                sensor->asserted_event.lower_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW_CRIT;
            }
            else if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->lower_noncritical_thr) + (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_critical_go_high){
                    ev = IPMI_THRESHOLD_LNC_GH;
                    sensor->asserted_event.lower_non_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_NORMAL;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) >= ((int8_t)sdr->lower_noncritical_thr)){
                ev = IPMI_THRESHOLD_LNC_GL;
                sensor->asserted_event.lower_non_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->lower_noncritical_thr) - (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.lower_non_critical_go_low){
                    ev = IPMI_THRESHOLD_LNC_GL;
                    sensor->asserted_event.lower_non_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        else{
            if(sensor->readout_value <= sdr->lower_critical_thr){
                ev = IPMI_THRESHOLD_LC_GH;
                sensor->asserted_event.lower_critical_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW_CRIT;
            }
            else if(sensor->readout_value >= (sdr->lower_noncritical_thr + (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_critical_go_high){
                    ev = IPMI_THRESHOLD_LNC_GH;
                    sensor->asserted_event.lower_non_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_NORMAL;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value >= sdr->lower_noncritical_thr){
                ev = IPMI_THRESHOLD_LNC_GL;
                sensor->asserted_event.lower_non_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value <= (sdr->lower_noncritical_thr - (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.lower_non_critical_go_low){
                    ev = IPMI_THRESHOLD_LNC_GL;
                    sensor->asserted_event.lower_non_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        break;

    case SENSOR_STATE_LOW_CRIT:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) <= ((int8_t)sdr->lower_nonrecover_thr)){
                ev = IPMI_THRESHOLD_LNR_GH;
                sensor->asserted_event.lower_non_recorverable_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW_NON_REC;
            }
            else if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->lower_critical_thr) + (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_critical_go_high){
                    ev = IPMI_THRESHOLD_LC_GH;
                    sensor->asserted_event.lower_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_LOW;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) >= ((int8_t)sdr->lower_critical_thr)){
                ev = IPMI_THRESHOLD_LC_GL;
                sensor->asserted_event.lower_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->lower_critical_thr) - (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.lower_critical_go_low){
                    ev = IPMI_THRESHOLD_LC_GL;
                    sensor->asserted_event.lower_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        else{
            if(sensor->readout_value <= sdr->lower_nonrecover_thr){
                ev = IPMI_THRESHOLD_LNR_GH;
                sensor->asserted_event.lower_non_recorverable_go_high = 1;
                ev_type = ASSERTION_EVENT;
                sensor->state = SENSOR_STATE_LOW_NON_REC;
            }
            else if(sensor->readout_value >= (sdr->lower_critical_thr + (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_critical_go_high){
                    ev = IPMI_THRESHOLD_LC_GH;
                    sensor->asserted_event.lower_critical_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_LOW;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value >= sdr->lower_critical_thr){
                ev = IPMI_THRESHOLD_LC_GL;
                sensor->asserted_event.lower_critical_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value <= (sdr->lower_critical_thr - (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.lower_critical_go_low){
                    ev = IPMI_THRESHOLD_LC_GL;
                    sensor->asserted_event.lower_critical_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        break;

    case SENSOR_STATE_LOW_NON_REC:
        if(sensor->signed_flag){
            if(((int8_t)sensor->readout_value) >= (((int8_t)sdr->lower_nonrecover_thr) + (1+((int8_t)sdr->pos_thr_hysteresis)))){
                if(sensor->asserted_event.upper_non_recoverable_go_high){
                    ev = IPMI_THRESHOLD_LNR_GH;
                    sensor->asserted_event.lower_non_recorverable_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_LOW_CRIT;
            }

            if(sensor->old_state != sensor->state && ((int8_t)sensor->readout_value) >= ((int8_t)sdr->lower_nonrecover_thr)){
                ev = IPMI_THRESHOLD_LNR_GL;
                sensor->asserted_event.lower_non_recoverable_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(((int8_t)sensor->readout_value) <= (((int8_t)sdr->lower_critical_thr) - (1+((int8_t)sdr->neg_thr_hysteresis)))){
                if(sensor->asserted_event.lower_non_recoverable_go_low){
                    ev = IPMI_THRESHOLD_LNR_GL;
                    sensor->asserted_event.lower_non_recoverable_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        else{
            if(sensor->readout_value >= (sdr->lower_nonrecover_thr + (1+sdr->pos_thr_hysteresis))){
                if(sensor->asserted_event.upper_non_recoverable_go_high){
                    ev = IPMI_THRESHOLD_LNR_GH;
                    sensor->asserted_event.lower_non_recorverable_go_high = 0;
                    ev_type = DEASSERTION_EVENT;
                }
                sensor->state = SENSOR_STATE_LOW_CRIT;
            }

            if(sensor->old_state != sensor->state && sensor->readout_value >= sdr->lower_nonrecover_thr){
                ev = IPMI_THRESHOLD_LNR_GL;
                sensor->asserted_event.lower_non_recoverable_go_low = 1;
                ev_type = ASSERTION_EVENT;
            }
            else if(sensor->readout_value <= (sdr->lower_critical_thr - (1+sdr->neg_thr_hysteresis))){
                if(sensor->asserted_event.lower_non_recoverable_go_low){
                    ev = IPMI_THRESHOLD_LNR_GL;
                    sensor->asserted_event.lower_non_recoverable_go_low = 0;
                    ev_type = DEASSERTION_EVENT;
                }
            }
        }
        break;
    }

    sensor->old_state = sensor->state;

    if ((ev != 0xFF) && (event_receiver_addr != 0xFF)) {
        ipmi_event_send(sensor, ev_type, &ev, 1);
    }
}

/* Management Controller Device Locator Record 37.9 SDR Type 12h */

const SDR_type_12h_t SDR0 = {
    .hdr.recID_LSB = 0x00,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51, /* IPMI protocol version */
    .hdr.rectype = TYPE_12, /* record type: device locator record */
    .hdr.reclength = sizeof(SDR_type_12h_t) - sizeof(SDR_entry_hdr_t),

/* record key bytes */
    .slaveaddr = 0x00,
    .chnum = 0x00,
    .power_notification_global_init = 0x04,
    .device_cap = 0x3b,
    .reserved[0] = 0x00,
    .reserved[1] = 0x00,
    .reserved[2] = 0x00,
    .entityID = 0xC1,
    .entityinstance = 0x00,
    .OEM = 0x00,
    .IDtypelen = 0xc0 | STR_SIZE(STR(TARGET_BOARD_NAME)), /* 8 bit ASCII, number of bytes */
    .IDstring = STR(TARGET_BOARD_NAME)
};
