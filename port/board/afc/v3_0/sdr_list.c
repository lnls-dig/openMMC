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

/* Project Includes */
#include "sdr.h"
#include "utils.h"
#include "i2c_mapping.h"

/* Sensors includes */
#include "sensors.h"
#include "fpga_spi.h"

/* SDR List */
#ifdef MODULE_INA220_VOLTAGE
#include "ina220.h"
#define SDR_FMC1_12V_ID        "FMC1 +12V"
#define SDR_FMC1_VADJ_ID       "FMC1 VADJ"
#define SDR_FMC1_P3V3_ID       "FMC1 +3.3V"
#define SDR_FMC2_12V_ID        "FMC2 +12V"
#define SDR_FMC2_VADJ_ID       "FMC2 VADJ"
#define SDR_FMC2_P3V3_ID       "FMC2 +3.3V"
#endif
#ifdef MODULE_INA220_CURRENT
#include "ina220.h"
#define SDR_FMC1_VADJ_CURR_ID  "FMC1 +12V Curr"
#define SDR_FMC1_12V_CURR_ID   "FMC1 VADJ Curr"
#define SDR_FMC1_P3V3_CURR_ID  "FMC1 +3.3V Curr"
#define SDR_FMC2_VADJ_CURR_ID  "FMC2 +12V Curr"
#define SDR_FMC2_12V_CURR_ID   "FMC2 VADJ Curr"
#define SDR_FMC2_P3V3_CURR_ID  "FMC2 +3.3V Curr"
#endif

#ifdef MODULE_HOTSWAP
/* AMC Hot-Swap sensor SDR */
const SDR_type_02h_t SDR_HOTSWAP_AMC = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_02,
    .hdr.reclength = sizeof(SDR_type_02h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

/* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x03, /* init: event generation + scanning enabled */
    .sensorcap = 0xc1, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_HOT_SWAP, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x6f, /* sensor reading*/
    .assertion_event_mask = { 0x00, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x00, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readable Threshold mask: no thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: no thresholds are setable: */
    .sensor_units_1 = 0xc0, /* sensor units 1 : Does not return analog reading*/
    .sensor_units_2 = 0x00, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .record_sharing[0] = 0x00,
    .record_sharing[1] = 0x00,
    .pos_thr_hysteresis = 0x00, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 0x00, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .reserved3 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("HOTSWAP AMC"), /* 8 bit ASCII, number of bytes */
    .IDstring = "HOTSWAP AMC" /* sensor string */
};

#ifdef MODULE_RTM
const SDR_type_02h_t SDR_HOTSWAP_RTM = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_02,
    .hdr.reclength = sizeof(SDR_type_02h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

/* record body bytes */
    .entityID = 0xC0, /* entity id: RTM */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x03, /* init: event generation + scanning enabled */
    .sensorcap = 0xc1, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_HOT_SWAP, /* sensor type: HOT SWAP*/
    .event_reading_type = 0x6f, /* sensor reading*/
    .assertion_event_mask = { 0x00, /* LSB assert event mask: 3 bit value */
                              0x00 }, /* MSB assert event mask */
    .deassertion_event_mask = { 0x00, /* LSB deassert event mask: 3 bit value */
                                0x00 }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x00, /* LSB: readable Threshold mask: no thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: no thresholds are setable: */
    .sensor_units_1 = 0xc0, /* sensor units 1 : Does not return analog reading*/
    .sensor_units_2 = 0x00, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .record_sharing[0] = 0x00,
    .record_sharing[1] = 0x00,
    .pos_thr_hysteresis = 0x00, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 0x00, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .reserved3 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("HOTSWAP RTM"), /* 8 bit ASCII, number of bytes */
    .IDstring = "HOTSWAP RTM" /* sensor string */
};
#endif
#endif

#ifdef MODULE_INA220_VOLTAGE
/* FMC1 12V */
const SDR_type_01h_t SDR_FMC1_12V = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 190, /* Nominal reading = 12.285V */
    .normal_max = 200, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 205, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 200, /* Upper critical Threshold */
    .upper_noncritical_thr = 190, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 170, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 175, /* Lower critical Threshold */
    .lower_noncritical_thr = 180, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_12V_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_12V_ID /* sensor string */
};

/* FMC1 PVADJ */
const SDR_type_01h_t SDR_FMC1_VADJ = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 39, /* Nominal reading */
    .normal_max = 47, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 51, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 46, /* Upper critical Threshold */
    .upper_noncritical_thr = 43, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 28, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 32, /* Lower critical Threshold */
    .lower_noncritical_thr = 36, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_VADJ_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_VADJ_ID /* sensor string */
};

/* FMC1 P3V3 */
const SDR_type_01h_t SDR_FMC1_P3V3 = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: VOLTAGE*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 52, /* Nominal reading */
    .normal_max = 59, /* Normal maximum */
    .normal_min = 45, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 62, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 59, /* Upper critical Threshold */
    .upper_noncritical_thr = 55, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 42, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 45, /* Lower critical Threshold */
    .lower_noncritical_thr = 48, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_P3V3_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_P3V3_ID /* sensor string */
};

/* FMC2 12V */
const SDR_type_01h_t SDR_FMC2_12V = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 190, /* Nominal reading = 12.285V */
    .normal_max = 200, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 205, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 200, /* Upper critical Threshold */
    .upper_noncritical_thr = 195, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 170, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 175, /* Lower critical Threshold */
    .lower_noncritical_thr = 180, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_12V_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_12V_ID /* sensor string */
};

/* FMC2 PVADJ */
const SDR_type_01h_t SDR_FMC2_VADJ = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 39, /* Nominal reading */
    .normal_max = 47, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 51, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 46, /* Upper critical Threshold */
    .upper_noncritical_thr = 43, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 28, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 32, /* Lower critical Threshold */
    .lower_noncritical_thr = 36, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_VADJ_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_VADJ_ID /* sensor string */
};

/* FMC2 P3V3 */
const SDR_type_01h_t SDR_FMC2_P3V3 = {

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
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: VOLTAGE*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 52, /* Nominal reading */
    .normal_max = 59, /* Normal maximum */
    .normal_min = 45, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 62, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 59, /* Upper critical Threshold */
    .upper_noncritical_thr = 55, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 42, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 45, /* Lower critical Threshold */
    .lower_noncritical_thr = 48, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_P3V3_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_P3V3_ID /* sensor string */
};
#endif

#ifdef MODULE_INA220_CURRENT
/* FMC1 12V Current */
const SDR_type_01h_t SDR_FMC1_12V_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 190, /* Nominal reading = 12.285V */
    .normal_max = 156, /* Normal maximum - 5A per sensor */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_12V_CURR_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_12V_CURR_ID /* sensor string */
};

/* FMC1 PVADJ Current */
const SDR_type_01h_t SDR_FMC1_VADJ_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 32, /* M -> Current LSB*20 */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 39, /* Nominal reading */
    .normal_max = 47, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0x7F, /* Sensor Maximum reading */
    .sensor_min_reading = 0x80, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_VADJ_CURR_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_VADJ_CURR_ID /* sensor string */
};

/* FMC1 P3V3 Current */
const SDR_type_01h_t SDR_FMC1_P3V3_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: VOLTAGE*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 52, /* Nominal reading */
    .normal_max = 59, /* Normal maximum */
    .normal_min = 45, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC1_P3V3_CURR_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC1_P3V3_CURR_ID /* sensor string */
};

/* FMC2 12V Current */
const SDR_type_01h_t SDR_FMC2_12V_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 190, /* Nominal reading = 12.285V */
    .normal_max = 200, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_12V_CURR_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_12V_CURR_ID /* sensor string */
};

/* FMC2 PVADJ Current */
const SDR_type_01h_t SDR_FMC2_VADJ_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 39, /* Nominal reading */
    .normal_max = 47, /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_VADJ_CURR_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_VADJ_CURR_ID /* sensor string */
};

/* FMC2 P3V3 Current */
const SDR_type_01h_t SDR_FMC2_P3V3_CURR = {

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
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: VOLTAGE*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = { 0xFF, /* LSB assert event mask: 3 bit value */
                              0x0F }, /* MSB assert event mask */
    .deassertion_event_mask = { 0xFF, /* LSB deassert event mask: 3 bit value */
                                0x0F }, /* MSB deassert event mask */
    .readable_threshold_mask = 0x3F, /* LSB: readabled Threshold mask: all thresholds are readabled:  */
    .settable_threshold_mask = 0x3F, /* MSB: setabled Threshold mask: all thresholds are setabled: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 52, /* Nominal reading */
    .normal_max = 59, /* Normal maximum */
    .normal_min = 45, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 100, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 90, /* Upper critical Threshold */
    .upper_noncritical_thr = 80, /* Upper non critical Threshold */
    .lower_nonrecover_thr = -60, /* Lower non-recoverable Threshold */
    .lower_critical_thr = -50, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC2_P3V3_CURR_ID), /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC2_P3V3_CURR_ID /* sensor string */
};
#endif

#ifdef MODULE_LM75
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
#endif

#ifdef MODULE_MAX6642
const SDR_type_01h_t SDR_MAX6642_FPGA = {

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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0x00, /* R-Exp , B-Exp */
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
    .IDtypelen = 0xc0 | STR_SIZE("TEMP FPGA"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP FPGA" /*  sensor string */
};
#endif

void user_sdr_init( void )
{
    /* Hotswap Sensor */
    sdr_insert_entry( TYPE_02, (void *) &SDR_HOTSWAP_AMC, &vTaskHotSwap_Handle, 0, 0 );

    /* INA220 sensors */
#ifdef MODULE_INA220_VOLTAGE
    /* FMC1 Voltage */
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_12V, &vTaskINA220_Handle, FMC1_12V_DEVID, CHIP_ID_INA_5 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_VADJ, &vTaskINA220_Handle, FMC1_VADJ_DEVID, CHIP_ID_INA_1 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_P3V3, &vTaskINA220_Handle, FMC1_P3V3_DEVID, CHIP_ID_INA_3 );

    /* FMC2 Voltage */
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_12V, &vTaskINA220_Handle, FMC2_12V_DEVID, CHIP_ID_INA_0 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_VADJ, &vTaskINA220_Handle, FMC2_VADJ_DEVID, CHIP_ID_INA_2 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_P3V3, &vTaskINA220_Handle, FMC2_P3V3_DEVID, CHIP_ID_INA_4 );
#endif

#ifdef MODULE_INA220_CURRENT
    /* FMC1 Current */
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_12V_CURR, &vTaskINA220_Handle, FMC1_12V_CURR_DEVID, CHIP_ID_INA_5 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_VADJ_CURR, &vTaskINA220_Handle, FMC1_VADJ_CURR_DEVID, CHIP_ID_INA_1 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC1_P3V3_CURR, &vTaskINA220_Handle, FMC1_P3V3_CURR_DEVID, CHIP_ID_INA_3 );

    /* FMC2 Current */
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_12V_CURR, &vTaskINA220_Handle, FMC2_12V_CURR_DEVID, CHIP_ID_INA_0 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_VADJ_CURR, &vTaskINA220_Handle, FMC2_VADJ_CURR_DEVID, CHIP_ID_INA_2 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC2_P3V3_CURR, &vTaskINA220_Handle, FMC2_P3V3_CURR_DEVID, CHIP_ID_INA_4 );
#endif

#ifdef MODULE_MAX6642
    sdr_insert_entry( TYPE_01, (void *) &SDR_MAX6642_FPGA, &vTaskMAX6642_Handle, 0, CHIP_ID_MAX6642 );
#endif

#ifdef MODULE_LM75
    /* Board Temperature */
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_uC, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_0 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_CLOCK_SWITCH, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_1 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_DCDC, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_2 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_RAM, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_3 );
#endif
}
