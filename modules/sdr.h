/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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

#ifndef SDR_H_
#define SDR_H_

#include "ipmb.h"

#define NUM_SENSOR                      21      /* Number of sensors */
#define NUM_SDR                         (NUM_SENSOR+1)  /* Number of SDRs */

/* Sensor Types */
#define SENSOR_TYPE_TEMPERATURE         0x01
#define SENSOR_TYPE_VOLTAGE             0x02
#define SENSOR_TYPE_CURRENT             0x03
#define SENSOR_TYPE_FAN                 0x04
#define SENSOR_TYPE_WATCHDOG            0x23
#define SENSOR_TYPE_VERSION_CHANGE      0x2B
#define SENSOR_TYPE_HOT_SWAP            0xF2

/* Assertion Event Codes */
#define ASSERTION_EVENT                 0x00
#define DEASSERTION_EVENT               0x80

/* Sensor States */
#define SENSOR_STATE_NORMAL             0x00    // temperature is in normal range
#define SENSOR_STATE_LOW                0x01    // temperature is below lower non critical
#define SENSOR_STATE_LOW_CRIT           0x02    // temperature is below lower critical
#define SENSOR_STATE_LOW_NON_REC        0x04    // temperature is below lower non recoverable
#define SENSOR_STATE_HIGH               0x08    // temperature is higher upper non critical
#define SENSOR_STATE_HIGH_CRIT          0x10    // temperature is higher upper critical
#define SENSOR_STATE_HIGH_NON_REC       0x20    // temperature is higher high non recoverable

/* IPMI Sensor Events */
#define IPMI_THRESHOLD_LNC_GL           0x00    // lower non critical going low
#define IPMI_THRESHOLD_LNC_GH           0x01    // lower non critical going high
#define IPMI_THRESHOLD_LC_GL            0x02    // lower critical going low
#define IPMI_THRESHOLD_LC_GH            0x03    // lower critical going HIGH
#define IPMI_THRESHOLD_LNR_GL           0x04    // lower non recoverable going low
#define IPMI_THRESHOLD_LNR_GH           0x05    // lower non recoverable going high
#define IPMI_THRESHOLD_UNC_GL           0x06    // upper non critical going low
#define IPMI_THRESHOLD_UNC_GH           0x07    // upper non critical going high
#define IPMI_THRESHOLD_UC_GL            0x08    // upper critical going low
#define IPMI_THRESHOLD_UC_GH            0x09    // upper critical going HIGH
#define IPMI_THRESHOLD_UNR_GL           0x0A    // upper non recoverable going low
#define IPMI_THRESHOLD_UNR_GH           0x0B    // upper non recoverable going high

/* Constants for comparison function */
#define UNSIGNED	0x00
#define SIGNED		0x01

#define LOWER_EQ	0x00
#define UPPER_EQ	0x01


typedef enum {
    TYPE_01 = 0x1,
    TYPE_02 = 0x2,
    TYPE_11 = 0x11,
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
    uint16_t assertion_event_mask;
    uint16_t deassertion_event_mask;
    uint8_t settable_threshold_mask;
    uint8_t readable_threshold_mask;
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
} SDR_type_01h_t; /* Full sensor */

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
    uint16_t assertion_event_mask;
    uint16_t deassertion_event_mask;
    uint8_t settable_threshold_mask;
    uint8_t readable_threshold_mask;
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
} SDR_type_02h_t; /* Compact Sensor */

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

typedef struct sensor_t {
    uint8_t num;
    SDR_TYPE sdr_type;
    void * sdr;
    uint8_t sdr_length;
    uint8_t diag_devID;
    uint8_t state;
    uint8_t old_state;
    uint16_t readout_value;
    uint8_t chipid;
    uint8_t signed_flag;
    uint8_t event_scan;
    uint8_t ownerID; /* This field is repeated here because its value is assigned during initialization, so it can't be const */
    uint8_t entityinstance; /* This field is repeated here because its value is assigned during initialization, so it can't be const */
    TaskHandle_t * task_handle;
    struct {
        uint16_t upper_non_recoverable_go_high:1;
        uint16_t upper_non_recoverable_go_low:1;
        uint16_t upper_critical_go_high:1;
        uint16_t upper_critical_go_low:1;
        uint16_t upper_non_critical_go_high:1;
        uint16_t upper_non_critical_go_low:1;
        uint16_t lower_non_recoverable_go_high:1;
        uint16_t lower_non_recoverable_go_low:1;
        uint16_t lower_critical_go_high:1;
        uint16_t lower_critical_go_low:1;
        uint16_t lower_non_critical_go_high:1;
        uint16_t lower_non_critical_go_low:1;
    } asserted_event;
    void* settings;
    struct sensor_t *next;
} sensor_t;

extern volatile uint8_t sdr_count;
extern sensor_t *sdr_head;
extern sensor_t *sdr_tail;

extern const SDR_type_12h_t SDR0;
extern const SDR_type_12h_t SDR_RTM_DEV_LOCATOR;

#define GET_SENSOR_TYPE(sensor)     ((SDR_type_01h_t *)sensor->sdr)->sensortype

#define GET_EVENT_TYPE_CODE(n)      ((SDR_type_01h_t *)sensor->sdr)->event_reading_type

void sdr_init( void );
void amc_sdr_init( void );
#ifdef MODULE_RTM
void rtm_sdr_init( void );
#endif
void sensor_init( void );
void sensor_enable(sensor_t *sensor);
void sensor_disable(sensor_t *sensor);
void check_sensor_event( sensor_t * sensor );
void sensor_state_check( sensor_t *sensor );
sensor_t * sdr_insert_entry( SDR_TYPE type, void * sdr, TaskHandle_t *monitor_task, uint8_t diag_id, uint8_t slave_addr);
void sdr_remove_entry( sensor_t * entry );
void sdr_pop( void );
sensor_t * sdr_add_settings(uint8_t chipid, void * settings);
sensor_t * find_sensor_by_sdr( void * sdr );
sensor_t * find_sensor_by_id( uint8_t id );

#endif
