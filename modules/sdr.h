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

#ifndef IPMI_SDR_H_
#define IPMI_SDR_H_

#include "ipmb.h"

#define NUM_SENSOR              9       /* Number of sensors */
#define NUM_SDR                 (NUM_SENSOR+1)  /* Number of SDRs */

#define HOT_SWAP_SENSOR         1
#define NUM_SDR_FMC2_12V        2
#define NUM_SDR_FMC2_VADJ       3
#define NUM_SDR_FMC2_3V3        4
#define NUM_SDR_FMC1_12V        5
#define NUM_SDR_FMC1_VADJ       6
#define NUM_SDR_FMC1_3V3        7
#define NUM_SDR_LM75_1          8
#define NUM_SDR_LM75_2          9
#define NUM_SDR_LM75_3          10
#define NUM_SDR_LM75_4          11

/* Sensor Types */
#define SENSOR_TYPE_TEMPERATURE                 0x01
#define SENSOR_TYPE_VOLTAGE                     0x02
#define SENSOR_TYPE_CURRENT                     0x03
#define SENSOR_TYPE_FAN                         0x04
#define SENSOR_TYPE_WATCHDOG                    0x23
#define SENSOR_TYPE_VERSION_CHANGE              0x2B
#define SENSOR_TYPE_HOT_SWAP                    0xF2

/* Module handle sensor status */
#define HOT_SWAP_CLOSED                         0x00
#define HOT_SWAP_OPENED                         0x01
#define HOT_SWAP_QUIESCED                       0x02

#define HOT_SWAP_STATE_HANDLE_CLOSED            (1 << 0)
#define HOT_SWAP_STATE_HANDLE_OPENED            (1 << 1)
#define HOT_SWAP_STATE_QUIESCED                 (1 << 2)
#define HOT_SWAP_STATE_BP_SDOWN                 (1 << 3)
#define HOT_SWAP_STATE_BP_FAIL                  (1 << 4)
#define HOT_SWAP_STATE_URTM_PRSENT              (1 << 5)
#define HOT_SWAP_STATE_URTM_ABSENT              (1 << 6)
#define HOT_SWAP_STATE_URTM_COMPATIBLE          (1 << 7)
#define HOT_SWAP_STATE_URTM_INCOMPATIBLE        (1 << 0)

typedef enum {
    TYPE_01 = 0x1,
    TYPE_02 = 0x2,
    TYPE_12 = 0x12
} SDR_TYPE;

typedef struct {
    uint8_t recID_LSB;
    uint8_t recID_MSB;
    uint8_t SDRversion;
    uint8_t rectype;
    uint8_t reclength;
} SDR_entry_hdr_t;

typedef struct {
    SDR_entry_hdr_t hdr;
    uint8_t ownerID;
    uint8_t ownerLUN;
    uint8_t sensornum;
    uint8_t entityID;
    uint8_t entityinstance;
    uint8_t sensorinit;
    uint8_t sensorcap;
    uint8_t sensortype;
    uint8_t event_reading_type;
    uint8_t assertion_event_mask[2];
    uint8_t deassertion_event_mask[2];
    uint8_t readable_threshold_mask;
    uint8_t settable_threshold_mask;
    uint8_t sensor_units_1;
    uint8_t sensor_units_2;
    uint8_t sensor_units_3;
    uint8_t linearization;
    uint8_t M;
    uint8_t M_tol;
    uint8_t B;
    uint8_t B_accuracy;
    uint8_t acc_exp_sensor_dir;
    uint8_t Rexp_Bexp;
    uint8_t analog_flags;
    uint8_t nominal_reading;
    uint8_t normal_max;
    uint8_t normal_min;
    uint8_t sensor_max_reading;
    uint8_t sensor_min_reading;
    uint8_t upper_nonrecover_thr;
    uint8_t upper_critical_thr;
    uint8_t upper_noncritical_thr;
    uint8_t lower_nonrecover_thr;
    uint8_t lower_critical_thr;
    uint8_t lower_noncritical_thr;
    uint8_t pos_thr_hysteresis;
    uint8_t neg_thr_hysteresis;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t OEM;
    uint8_t IDtypelen;
    char IDstring[16];      // bits 49-64 (0x40 length max)
} SDR_type_01h_t; /* Temperature Sensor */

typedef struct {
    SDR_entry_hdr_t hdr;
    // RECORD KEY BYTES
    uint8_t ownerID;
    uint8_t ownerLUN;
    uint8_t sensornum;
    // record body bytes
    uint8_t entityID;
    uint8_t entityinstance;
    uint8_t sensorinit;
    uint8_t sensorcap;
    uint8_t sensortype;
    uint8_t event_reading_type;
    uint8_t assertion_event_mask[2];
    uint8_t deassertion_event_mask[2];
    uint8_t readable_threshold_mask;
    uint8_t settable_threshold_mask;
    uint8_t sensor_units_1;
    uint8_t sensor_units_2;
    uint8_t sensor_units_3;
    uint8_t record_sharing[2];
    uint8_t pos_thr_hysteresis;
    uint8_t neg_thr_hysteresis;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t OEM;
    uint8_t IDtypelen;
    char IDstring[16];     // bits 32-64 (0x40 length max)
} SDR_type_02h_t; /* Voltage Sensor */

typedef struct {
    SDR_entry_hdr_t hdr;
    uint8_t slaveaddr;
    uint8_t chnum;
    uint8_t power_notification_global_init;
    uint8_t device_cap;
    uint8_t reserved[3];
    uint8_t entityID;
    uint8_t entityinstance;
    uint8_t OEM;
    uint8_t IDtypelen;
    char IDstring[16];
} SDR_type_12h_t;

typedef struct {
    uint8_t ownerID;
    uint8_t entityID;
    uint8_t entityinstance;
    uint16_t readout_value;
    uint8_t comparator_status;
    void * sensor_info;
} sensor_data_entry_t;

typedef struct {
    SDR_TYPE type;
    void * sdr;
    uint8_t sdr_length;
    sensor_data_entry_t * data;
    TaskHandle_t * task_handle;
    uint8_t slave_addr;
    uint8_t diag_devID;
} sensor_t;

extern sensor_data_entry_t sdrData[NUM_SDR];
extern const sensor_t const sensor_array[NUM_SDR];

void ipmi_se_get_sdr( ipmi_msg *req, ipmi_msg* rsp);
void ipmi_se_get_sensor_reading( ipmi_msg *req, ipmi_msg* rsp);
void ipmi_se_get_sdr_info( ipmi_msg *req, ipmi_msg* rsp);
void ipmi_se_reserve_device_sdr( ipmi_msg *req, ipmi_msg* rsp);
//void ipmi_se_set_receiver ( ipmi_msg *req, ipmi_msg *rsp );

void initializeDCDC();
void do_quiesced_init();
void do_quiesced(unsigned char ctlcode);
void sdr_init(uint8_t ipmiID);
void sensor_init( void );

#endif
