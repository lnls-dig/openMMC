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

#include "sdr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pin_mapping.h"
#include "i2c.h"
#include "port.h"
#include "ipmi.h"
#include "ipmb.h"
#include "payload.h"
#include "board_version.h"
#include "led.h"
#include "task_priorities.h"

uint16_t INA220_readVolt(I2C_ID_T i2c, uint8_t address, bool raw);

/* Management Controller Device Locator Record 37.9 SDR Type 12h */
static const SDR_type_12h_t  SDR0 = {
    .hdr.recID_LSB = 0x00, /* record number, LSB - filled by sdr_init() */
    .hdr.recID_MSB = 0x00, /* record number, MSB - filled by sdr_init() */
    .hdr.SDRversion = 0x51, /* IPMI protocol version */
    .hdr.rectype = TYPE_12, /* record type: device locator record */
    .hdr.reclength = sizeof(SDR_type_12h_t) - sizeof(SDR_entry_hdr_t), /* record length - filled by sdr_init() */

/* record key bytes */
    .slaveaddr = 0x00, // owner ID??
    .chnum = 0x00,
    .power_notification_global_init = NUM_SDR,
    .device_cap = 0x3b,
    .reserved[0] = 0x00,
    .reserved[1] = 0x00,
    .reserved[2] = 0x00,
    .entityID = 0x60, //0x60 | ((0x76-0x70) >>1), // Entry ID?
    .entityinstance = 0x00,
    .OEM = 0x00,
    .IDtypelen = 0xc7, /* 8 bit ASCII, number of bytes */
    .IDstring = {'A', 'F', 'C', ' ', 'I', 'P', 'M' }
};

/* Hot-Swap sensor */
static const SDR_type_02h_t SDR_HOT_SWAP = {
    .hdr.recID_LSB = HOT_SWAP_SENSOR,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_02,
    .hdr.reclength = sizeof(SDR_type_02h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = HOT_SWAP_SENSOR, /* sensor number */

/* record body bytes */
    .entityID = 0x60, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x03, /* init: event generation + scanning enabled */
    .sensorcap = 0xc1, /* capabilities: auto re-arm,*/
    .sensortype = HOT_SWAP, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x6f, /* sensor reading*/
    .assertion_event_mask = { 0x07, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x07, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: all thresholds are setable: */
    .sensor_units_1 = 0xc0, /* sensor units 1 :*/
    .sensor_units_2 = 0x00, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .record_sharing[0] = 0x00, /* Linearization */
    .record_sharing[1] = 0x00, /* Linearization */
    .pos_thr_hysteresis = 0x00, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 0x00, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .reserved3 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xcc, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F', 'R', 'U', ' ', 'H', 'O', 'T', '_', 'S', 'W', 'A', 'P' } /* sensor string */
};

/* 12V sensor */
static const SDR_type_01h_t SDR_FMC2_12V = {

    .hdr.recID_LSB = NUM_SDR_FMC2_12V,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC2_12V, /* sensor number */

    /* record body bytes */
    .entityID = 0x60, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = 0x02, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0x07, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x07, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x00, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0xc0, /* sensor units 1 :*/
    .sensor_units_2 = 0x4, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 63, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 195, /* Nominal reading */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 250, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 236, /* Upper critical Threshold */
    .upper_noncritical_thr = 225, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 193, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 174, /* Lower critical Threshold */
    .lower_noncritical_thr = 178, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 4, /* 8 bit ASCII, number of bytes */
    .IDstring = { '+', '1', '2', 'V' } /* sensor string */
};

/* FMC2 PVADJ sensor */
static const SDR_type_01h_t SDR_FMC2_VADJ = {

    .hdr.recID_LSB = NUM_SDR_FMC2_VADJ,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC2_VADJ, /* sensor number */

    /* record body bytes */
    .entityID = 0x60, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = 0x02, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0x07, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x07, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x00, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0xc0, /* sensor units 1 :*/
    .sensor_units_2 = 0x4, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 63, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 195, /* Nominal reading */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 250, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 236, /* Upper critical Threshold */
    .upper_noncritical_thr = 225, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 193, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 174, /* Lower critical Threshold */
    .lower_noncritical_thr = 178, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 9, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','2',' ', 'V', 'A', 'D', 'J' } /* sensor string */
};


/* FMC2 PVADJ sensor */
static const SDR_type_01h_t SDR_FMC2_P3V3 = {

    .hdr.recID_LSB = NUM_SDR_FMC2_3V3,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC2_3V3, /* sensor number */

    /* record body bytes */
    .entityID = 0x60, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = 0x02, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0x07, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x07, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x00, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0xc0, /* sensor units 1 :*/
    .sensor_units_2 = 0x4, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 63, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 195, /* Nominal reading */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 250, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 236, /* Upper critical Threshold */
    .upper_noncritical_thr = 225, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 193, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 174, /* Lower critical Threshold */
    .lower_noncritical_thr = 178, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 9, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','2',' ', '+', '3', 'V', '3' } /* sensor string */
};



static const INA220_params_t PARAMS_FMC2_VADJ = {
    .i2c = I2C1,
    .address = 0x41,
};
static const INA220_params_t PARAMS_FMC2_P3V3 = {
    .i2c = I2C1,
    .address = 0x43,
};

void sdr_sensor_readout_INA220(const struct sensor_ * params) {
    if (params == NULL) {
        return;
    }

    INA220_params_t * tmp_params = (INA220_params_t *) params->callback_params;
    if (tmp_params == NULL) {
        return;
    }

    uint16_t val = INA220_readVolt(tmp_params->i2c, tmp_params->address, true) / 16 ;

    params->data->readout_value = val;
}

sensor_data_entry_t sdrData[NUM_SDR];
const sensor_t const sensor_array[NUM_SDR]  = {
    { TYPE_12, (void *) &SDR0,          sizeof(SDR0),          &sdrData[0] },
    { TYPE_02, (void *) &SDR_HOT_SWAP,  sizeof(SDR_HOT_SWAP),  &sdrData[1] },
    { TYPE_01, (void *) &SDR_FMC2_12V,  sizeof(SDR_FMC2_12V),  &sdrData[2] },
    { TYPE_01, (void *) &SDR_FMC2_VADJ, sizeof(SDR_FMC2_VADJ), &sdrData[3], sdr_sensor_readout_INA220,  (void *)&PARAMS_FMC2_VADJ},
    { TYPE_01, (void *) &SDR_FMC2_P3V3, sizeof(SDR_FMC2_P3V3), &sdrData[4], sdr_sensor_readout_INA220,  (void *)&PARAMS_FMC2_P3V3}
};

// @todo: check if valid
#define SDR_ARRAY_LENGTH (sizeof(sensor_array) / sizeof(sensor_t))

// @todo: check if valid
size_t sdr_get_size_by_type(SDR_TYPE type){
    switch (type) {
    case TYPE_01:
        return sizeof(SDR_type_01h_t);
        break;
    case TYPE_02:
        return sizeof(SDR_type_02h_t);
        break;
    case TYPE_12:
        return sizeof(SDR_type_12h_t);
        break;
    default:
        return 0;
        break;
    }

}

// @todo: check if valid
size_t sdr_get_size_by_entry(int id){
    if (id >= SDR_ARRAY_LENGTH) {
        return 0;
    }
    return sdr_get_size_by_type(sensor_array[id].type);
}

static uint16_t reservationID;

SemaphoreHandle_t semaphore_fru_control;

uint8_t payload_ctrl_code;

void do_quiesced_init() {
    payload_ctrl_code = 0;
    semaphore_fru_control = xSemaphoreCreateBinary();
}
void do_quiesced(unsigned char ctlcode){
    payload_ctrl_code = ctlcode;
    xSemaphoreGive(semaphore_fru_control);
}

void sdr_init(uint8_t ipmiID) {
    uint8_t i;
    for (i = 0; i < NUM_SDR; i++) {
        sensor_array[i].data->entityinstance =  0x60 | ((ipmiID - 0x70) >> 1);
        sensor_array[i].data->ownerID = ipmiID;

        // @todo: remove this HOT_SWAP_SENSOR case, to enable first event
        //if (i == HOT_SWAP_SENSOR) {
        //          sensor_array[i].data->comparator_status = HOT_SWAP_STATE_HANDLE_OPENED;
        //} else {
        sensor_array[i].data->comparator_status = 0;
        sensor_array[i].data->readout_value = 0;
        //}
    }
    xTaskCreate(vTaskSensor, "Sensor", configMINIMAL_STACK_SIZE, NULL, tskSENSOR_PRIORITY, (TaskHandle_t *) NULL );
}


#ifdef FREERTOS_CONFIG_H

#define SENSOR_DELAY_PERIOD 1000

I2C_Mutex i2c_mutex_array[2];

#define INA220_BUS_REG 0x02

// INA220 7bit address
// 0x40 - FMC2 P12V
// 0x41 - FMC2 PVADJ
// 0x43 - FMC2 P3V3

// 0x45 - FMC1 P12V
// 0x42 - FMC1 PVADJ
// 0x44 - FMC1 P3V3


static void INA220_init(I2C_ID_T i2c, uint8_t address)
{
    uint8_t ch[3];
    ch[0] = 0 ; // INA220_CFG_REG
    ch[1] = 0x01;
    ch[2] = 0x9f;
    xI2CMasterWrite(i2c, address, ch, 3);
}

uint16_t INA220_readVolt(I2C_ID_T i2c, uint8_t address, bool raw)
{
    uint8_t ch[2];
    xI2CMasterWriteRead(i2c, address, INA220_BUS_REG, ch, 2);
    uint16_t tmpVal = 0.0;
    tmpVal = (0x1fE0 & (ch[0] << 5)) | (0x1f & (ch[1] >> 3));
    if (raw == false) {
        tmpVal = tmpVal * 4;
    }
    return tmpVal;
}

void vTaskSensor( void *pvParmeters )
{
    SDR_type_01h_t *pSDR = NULL;
    sensor_data_entry_t * pDATA;
    I2C_ID_T i2c_bus_id;
    uint8_t i2c_address;

    if (afc_i2c_take_by_chipid(CHIP_ID_INA_0, &i2c_address,&i2c_bus_id, (TickType_t) 0 ) == pdTRUE ) {
        INA220_init(i2c_bus_id, i2c_address);
        afc_i2c_give(i2c_bus_id);
    } else {
        // fatal error
    }

    uint8_t i;
    for( ;; )
    {
        // @todo: move to payload task
        if (xSemaphoreTake(semaphore_fru_control, SENSOR_DELAY_PERIOD) == pdTRUE) {
            if (payload_ctrl_code == FRU_CTLCODE_QUIESCE) {
                sensor_array[HOT_SWAP_SENSOR].data->comparator_status |= HOT_SWAP_STATE_QUIESCED;

                ipmi_msg pmsg;
                int data_len = 0;

                pmsg.dest_LUN = 0;
                pmsg.netfn = NETFN_SE;
                pmsg.cmd = IPMI_PLATFORM_EVENT_CMD;
                pmsg.data[data_len++] = 0x04;
                pmsg.data[data_len++] = 0xf2;
                pmsg.data[data_len++] = HOT_SWAP_SENSOR;
                pmsg.data[data_len++] = 0x6f;
                pmsg.data[data_len++] = HOT_SWAP_QUIESCED; // hot swap state
                pmsg.data_len = data_len;
                ipmb_send_request( &pmsg );

            }
            continue;
        }
        /* @todo: add real delay checking */
        if (afc_i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t)100) == pdTRUE) {

            // @todo: add sensor check loop
            // @todo: add timestamping??
            // HotSwap
            i=1;
            pSDR = (SDR_type_01h_t *) sensor_array[i].sdr;
            pDATA = sensor_array[i].data;
            if (pSDR->sensornum == HOT_SWAP_SENSOR) {
                bool tmp_val = gpio_read_pin(HOT_SWAP_HANDLE_PORT, HOT_SWAP_HANDLE_PIN);
                uint8_t new_flag = 0;
                uint8_t old_flag = pDATA->comparator_status & 0x03;

                if (tmp_val) {
                    // handle opened
                    new_flag = HOT_SWAP_STATE_HANDLE_OPENED;
                } else {
                    // handle closed
                    new_flag = HOT_SWAP_STATE_HANDLE_CLOSED;
                }

                if (new_flag != old_flag) {
                    ipmi_msg pmsg;
		    int data_len = 0;

		    LED_activity_desc_t LEDact;
		    LEDact.action = LED_ACTV_ON;
		    LED_update( LED_RED, Override, &LEDact );
		    pmsg.dest_LUN = 0;
                    pmsg.netfn = NETFN_SE;
                    pmsg.cmd = IPMI_PLATFORM_EVENT_CMD;
                    pmsg.data[data_len++] = 0x04;
                    pmsg.data[data_len++] = 0xf2;
                    pmsg.data[data_len++] = pSDR->sensornum;
                    pmsg.data[data_len++] = 0x6f;
                    pmsg.data[data_len++] = (new_flag >> 1); // hot swap state
                    pmsg.data_len = data_len;
		    /* We must keep trying to send the hot-swap event message.
		     * If it fails, we don't update the sensor record,
		     * so it'll try again in the next unblock */
		    if (ipmb_send_request( &pmsg ) == ipmb_error_success) {
			pDATA->comparator_status = (pDATA->comparator_status & 0xFC) | new_flag;
		    }
                }
            }

            // 12v dcdc
            i = 2;
            pSDR = (SDR_type_01h_t *) sensor_array[i].sdr;
            pDATA = sensor_array[i].data;
            if (pSDR->sensornum == NUM_SDR_FMC2_12V) {
                pDATA->readout_value = INA220_readVolt(i2c_bus_id, 0x40, true) / 16;

                if (pDATA->readout_value > pSDR->lower_noncritical_thr) {
                    payload_send_message(PAYLOAD_MESSAGE_P12GOOD);
                } else if (pDATA->readout_value < pSDR->lower_critical_thr) {
                    payload_send_message(PAYLOAD_MESSAGE_P12GOODn);
                }
            }

            for (i=3; i<5; i++) {
                if (sensor_array[i].callback_function == NULL) {
		    continue;
		}
                sensor_array[i].callback_function(&sensor_array[i]);
            }

            afc_i2c_give(i2c_bus_id);
        }
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
void ipmi_se_set_receiver ( ipmi_msg *req, ipmi_msg *rsp) {

    /** @todo: actually enable/disable sending events*/
    rsp->completion_code = IPMI_CC_OK;
    rsp->data_len = 0;
}

void ipmi_se_get_sdr( ipmi_msg *req,  ipmi_msg* rsp) {
    ipmi_se_get_sdr_param_t * params;
    params = (ipmi_se_get_sdr_param_t *) req->data;

    uint16_t record_id = params->record_id[0] | (params->record_id[1] << 8);
    uint16_t recv_reserv_id = params->reservation_id[0] | (params->reservation_id[1] << 8);
    uint8_t len = rsp->data_len;
    uint8_t i;
    uint8_t index;

    /* Reservation ID check */
    if (reservationID != recv_reserv_id) {
        rsp->data_len = 0;
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        return;
    }

    if ( (record_id >= NUM_SDR)
         || (params->offset + params->size) > sensor_array[record_id].sdr_length) {
        rsp->completion_code = IPMI_CC_REQ_DATA_NOT_PRESENT;
        return;
    } else if (record_id == NUM_SDR - 1) {
        rsp->data[len++] = 0xFF;
        rsp->data[len++] = 0xFF;
    } else {
        rsp->data[len++] = (record_id + 1) & 0xff; /* next record ID */
        rsp->data[len++] = (record_id + 1) >> 8; /* next record ID */
    }

    SDR_entry_hdr_t *pHDR;
    uint8_t tmp_c;
    pHDR = (SDR_entry_hdr_t*) sensor_array[record_id].sdr;
    uint8_t * pSDR = (uint8_t*) sensor_array[record_id].sdr;

    for (i = 0; i < params->size; i++) {
        index = i + params->offset;
        tmp_c = pSDR[index];

        /* Read not-const data from data buffer filled by sdr_init/sensor_task */
        if (index == 5) {
            tmp_c = sensor_array[record_id].data->ownerID;
        } else if ( pHDR->rectype == 0x02 || pHDR->rectype == 0x01 ) {
            if (index == 9 ) {
                tmp_c = sensor_array[record_id].data->entityinstance;
            }
        }else if ( pHDR->rectype == 0x12 || pHDR->rectype == 0x11 ) {
            if (index == 13 ) {
                tmp_c = sensor_array[record_id].data->entityinstance;
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

    rsp->data[len++] = sensor_array[sensor_number].data->readout_value;
    /* Sensor scanning disabled */
    rsp->data[len++] = 0x40;
    rsp->data[len++] = sensor_array[sensor_number].data->comparator_status;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

#endif
