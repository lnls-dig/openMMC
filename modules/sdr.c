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
#include "sdr_list.h"
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
        sensor_array[i].data->entityinstance =  0x60 | ((ipmiID - 0x70) >> 1);
        sensor_array[i].data->ownerID = ipmiID;
        sensor_array[i].data->comparator_status = 0;
        sensor_array[i].data->readout_value = 0;
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

    uint8_t tmp_c;
    uint8_t * pSDR = (uint8_t*) sensor_array[record_id].sdr;

    for (uint8_t i = 0; i < size; i++) {
        tmp_c = pSDR[i+offset];
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

    rsp->data[len++] = sensor_array[sensor_number].data->readout_value;
    /* Sensor scanning disabled */
    rsp->data[len++] = 0x40;
    rsp->data[len++] = sensor_array[sensor_number].data->comparator_status;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}
