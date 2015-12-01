/*
 * sdr.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sdr.h"
#include "sensors.h"
#include "pin_mapping.h"
#include "i2c.h"
#include "port.h"
#include "ipmi.h"
#include "ipmb.h"
#include "payload.h"
#include "board_version.h"
#include "led.h"
#include "task_priorities.h"
#include "fpga_spi.h"


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

size_t sdr_get_size_by_entry(int id)
{
    if (id >= SDR_ARRAY_LENGTH) {
        return 0;
    }
    return sdr_get_size_by_type(sensor_array[id].type);
}

void sensor_init( void )
{
    hotswap_init();
    LM75_init();
    ina220_init();
}

static uint16_t reservationID;

SemaphoreHandle_t semaphore_fru_control;

uint8_t payload_ctrl_code;

void do_quiesced_init()
{
    payload_ctrl_code = 0;
    semaphore_fru_control = xSemaphoreCreateBinary();
}

void do_quiesced(unsigned char ctlcode)
{
    payload_ctrl_code = ctlcode;
    xSemaphoreGive(semaphore_fru_control);
}

void sdr_init(uint8_t ipmiID)
{
    uint8_t i;
    for (i = 0; i < NUM_SDR; i++) {
        if( i == 0) {
            sensor_array[i].slave_addr = ipmiID;
        } else {
            sensor_array[i].ownerID = ipmiID;
            sensor_array[i].readout_value = 0;
        }
        sensor_array[i].entityinstance =  0x60 | ((ipmiID - 0x70) >> 1);
    }
}

/******************************/
/* IPMI SDR Commands handlers */
/******************************/

void ipmi_se_get_sdr_info(ipmi_msg *req, ipmi_msg *rsp) {
    int len = rsp->data_len;

    if (req->data_len == 0 || req->data[0] == 0) {
        rsp->data[len++] = NUM_SENSOR;
    } else {
        rsp->data[len++] = NUM_SDR;
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
 * This handler should set (or reset) the address to which IPMI events
 * will be sent. Also, disable sending events if command 0xFF is received.
 *
 * @param[in] req Incoming request to be handled and answered.
 *
 * @return void
 */
void ipmi_se_set_receiver ( ipmi_msg *req, ipmi_msg *rsp)
{
    /** @todo: actually enable/disable sending events*/
    rsp->completion_code = IPMI_CC_OK;
    rsp->data_len = 0;
}

void ipmi_se_get_sdr( ipmi_msg *req,  ipmi_msg* rsp)
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

        if (index == 5) {
            tmp_c = sensor_array[record_id].ownerID;
        } else if ( sdr_type == TYPE_01 || sdr_type == TYPE_02 ) {
            if (index == 9) {
                tmp_c = sensor_array[record_id].entityinstance;
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

void ipmi_se_reserve_device_sdr( ipmi_msg *req, ipmi_msg* rsp) {
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

void ipmi_se_get_sensor_reading( ipmi_msg *req, ipmi_msg* rsp) {
    int sensor_number = req->data[0];
    int len = rsp->data_len;

    if (sensor_number >= NUM_SDR) {
        rsp->completion_code = IPMI_CC_REQ_DATA_NOT_PRESENT;
        rsp->data_len = 0;
        return;
    }

    rsp->data[len++] = sensor_array[sensor_number].readout_value;
    /* Sensor scanning disabled */
    rsp->data[len++] = 0x40;
    /* Present threshold status */
    /* TODO: Implement threshold reading */
    rsp->data[len++] = 0xC0;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

void check_sensor_event(uint8_t sensID)
{
    /** Should be rewritten to be compliant with RTM management !! */
    uint8_t ev = 0xFF;
    uint8_t ev_type;

    sensor_t * sensor = &sensor_array[sensID];
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

    if (ev != 0xFF) {
        ipmi_event_send(sensID, ev_type, &ev, 1);
    }
}
