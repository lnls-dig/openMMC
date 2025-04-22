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
#include "sensors.h"
#include "rtm_i2c_mapping.h"

/* Rename file to sdr_list.c */
#ifdef MODULE_HOTSWAP
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
    .assertion_event_mask = 0x0000, /* assertion event mask */
    .deassertion_event_mask = 0x0000, /* deassertion event mask */
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

#ifdef MODULE_LM75
const SDR_type_01h_t SDR_LM75_RTM_1 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC0, /* entity id: RTM */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
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
    .IDtypelen = 0xc0 | STR_SIZE("Air in"), /* 8 bit ASCII, number of bytes */
    .IDstring = "Air in" /* sensor string */
};

const SDR_type_01h_t SDR_LM75_RTM_2 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC0, /* entity id: RTM */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
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
    .IDtypelen = 0xc0 | STR_SIZE("Air out"), /* 8 bit ASCII, number of bytes */
    .IDstring = "Air out" /* sensor string */
};

const SDR_type_01h_t SDR_LM75_RTM_3 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityID = 0xC0, /* entity id: RTM */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x68, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_TEMPERATURE, /* sensor type */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
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
    .IDtypelen = 0xc0 | STR_SIZE("PS Block"), /* 8 bit ASCII, number of bytes */
    .IDstring = "PS Block" /* sensor string */
};
#endif

#ifdef MODULE_MAX11609
const SDR_type_01h_t SDR_MAX11609_VS1 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 20, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (3700/20), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 3.7 V */
    .normal_max = (5000/20), /* Normal maximum = 5 V */
    .normal_min = (3600/20), /* Normal minimum = 3.6 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (5119/20), /* Upper non-recoverable Threshold = 5.119 V */
    .upper_critical_thr = (5100/20), /* Upper critical Threshold = 5.1 V */
    .upper_noncritical_thr = (5050/20), /* Upper non critical Threshold = 5.05 V */
    .lower_nonrecover_thr = (3200/20), /* Lower non-recoverable Threshold = 3.2 V */
    .lower_critical_thr = (3300/20), /* Lower critical Threshold = 3.3 V */
    .lower_noncritical_thr = (3400/20), /* Lower non-critical Threshold = 3.4 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_0, /* ADC channel 0 */
    .IDtypelen = 0xc0 | STR_SIZE("VS1"), /* 8 bit ASCII, number of bytes */
    .IDstring = "VS1" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_VS2 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 20, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (3700/20), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 3.7 V */
    .normal_max = (5000/20), /* Normal maximum = 5 V */
    .normal_min = (3600/20), /* Normal minimum = 3.6 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (5119/20), /* Upper non-recoverable Threshold = 5.119 V */
    .upper_critical_thr = (5100/20), /* Upper critical Threshold = 5.1 V */
    .upper_noncritical_thr = (5050/20), /* Upper non critical Threshold = 5.05 V */
    .lower_nonrecover_thr = (3200/20), /* Lower non-recoverable Threshold = 3.2 V */
    .lower_critical_thr = (3300/20), /* Lower critical Threshold = 3.3 V */
    .lower_noncritical_thr = (3400/20), /* Lower non-critical Threshold = 3.4 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_1, /* ADC channel 1 */
    .IDtypelen = 0xc0 | STR_SIZE("VS2"), /* 8 bit ASCII, number of bytes */
    .IDstring = "VS2" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_N7V = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = (uint8_t)-40, /* M */
    .M_tol = 0xC0, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (7040/40), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 7.04 V */
    .normal_max = (7200/40), /* Normal maximum = 7.2 V */
    .normal_min = (6800/40), /* Normal minimum = 6.8 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (8500/40), /* Upper non-recoverable Threshold = 8.5 V */
    .upper_critical_thr = (8000/40), /* Upper critical Threshold = 8 V */
    .upper_noncritical_thr = (7500/40), /* Upper non critical Threshold = 7.5 V */
    .lower_nonrecover_thr = (5500/40), /* Lower non-recoverable Threshold = 5.5 V */
    .lower_critical_thr = (6000/40), /* Lower critical Threshold = 6 V */
    .lower_noncritical_thr = (6500/40), /* Lower non-critical Threshold = 6.5 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_2, /* ADC channel 2 */
    .IDtypelen = 0xc0 | STR_SIZE("N7V"), /* 8 bit ASCII, number of bytes */
    .IDstring = "N7V" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_7V = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 40, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (7000/40), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 7 V */
    .normal_max = (7200/40), /* Normal maximum = 7.2 V */
    .normal_min = (6800/40), /* Normal minimum = 6.8 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (8500/40), /* Upper non-recoverable Threshold = 8.5 V */
    .upper_critical_thr = (8000/40), /* Upper critical Threshold = 8 V */
    .upper_noncritical_thr = (7500/40), /* Upper non critical Threshold = 7.5 V */
    .lower_nonrecover_thr = (5500/40), /* Lower non-recoverable Threshold = 5.5 V */
    .lower_critical_thr = (6000/40), /* Lower critical Threshold = 6 V */
    .lower_noncritical_thr = (6500/40), /* Lower non-critical Threshold = 6.5 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_3, /* ADC channel 3 */
    .IDtypelen = 0xc0 | STR_SIZE("7V"), /* 8 bit ASCII, number of bytes */
    .IDstring = "7V" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_2V5 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 16, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (2480/16), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 2.48 V */
    .normal_max = (2600/16), /* Normal maximum = 2.6 V */
    .normal_min = (2400/16), /* Normal minimum = 2.4 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (2800/16), /* Upper non-recoverable Threshold = 2.8 V */
    .upper_critical_thr = (2700/16), /* Upper critical Threshold = 2.7 V */
    .upper_noncritical_thr = (2650/16), /* Upper non critical Threshold = 2.65 V */
    .lower_nonrecover_thr = (2200/16), /* Lower non-recoverable Threshold = 2.2 V */
    .lower_critical_thr = (2300/16), /* Lower critical Threshold = 2.3 V */
    .lower_noncritical_thr = (2350/16), /* Lower non-critical Threshold = 2.35 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_4, /* ADC channel 4 */
    .IDtypelen = 0xc0 | STR_SIZE("2V5"), /* 8 bit ASCII, number of bytes */
    .IDstring = "2V5" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_3V3 = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 16, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (3312/16), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 3.312 V */
    .normal_max = (3400/16), /* Normal maximum = 3.4 V */
    .normal_min = (3200/16), /* Normal minimum = 3.2 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (3700/16), /* Upper non-recoverable Threshold = 3.7 V */
    .upper_critical_thr = (3600/16), /* Upper critical Threshold = 3.6 V */
    .upper_noncritical_thr = (3500/16), /* Upper non critical Threshold = 3.5 V */
    .lower_nonrecover_thr = (2900/16), /* Lower non-recoverable Threshold = 2.9 V */
    .lower_critical_thr = (3000/16), /* Lower critical Threshold = 3 V */
    .lower_noncritical_thr = (3100/16), /* Lower non-critical Threshold = 3.1 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_5, /* ADC channel 5 */
    .IDtypelen = 0xc0 | STR_SIZE("3V3"), /* 8 bit ASCII, number of bytes */
    .IDstring = "3V3" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_5V = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x00, /* sensor units 1 :*/
    .sensor_units_2 = 0x04, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 32, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (5024/32), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 5.024 V */
    .normal_max = (4800/32), /* Normal maximum = 4.8 V */
    .normal_min = (5200/32), /* Normal minimum = 5.2 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (5500/32), /* Upper non-recoverable Threshold = 5.5 V */
    .upper_critical_thr = (5400/32), /* Upper critical Threshold = 5.4 V */
    .upper_noncritical_thr = (5300/32), /* Upper non critical Threshold = 5.3 V */
    .lower_nonrecover_thr = (4300/32), /* Lower non-recoverable Threshold = 4.3 V */
    .lower_critical_thr = (4500/32), /* Lower critical Threshold = 4.5 V */
    .lower_noncritical_thr = (4600/32), /* Lower non-critical Threshold = 4.6 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_6, /* ADC channel 5 */
    .IDtypelen = 0xc0 | STR_SIZE("5V"), /* 8 bit ASCII, number of bytes */
    .IDstring = "5V" /*  sensor string */
};

const SDR_type_01h_t SDR_MAX11609_12V_HP = {

    .hdr.recID_LSB = 0x00, /* Filled by sdr_insert_entry() */
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .entityID = 0xC0, /* entity id: AMC Module */
    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = 0x00, /* Filled by sdr_insert_entry() */

    /* record body bytes */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: Voltage*/
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
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
    .nominal_reading = (12416 >> 6), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] = 12.146 V */
    .normal_max = (13000 >> 6), /* Normal maximum = 13 V */
    .normal_min = (11000 >> 6), /* Normal minimum = 11 V */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (16000 >> 6), /* Upper non-recoverable Threshold = 16 V */
    .upper_critical_thr = (15000 >> 6), /* Upper critical Threshold = 15 V */
    .upper_noncritical_thr = (14000 >> 6), /* Upper non critical Threshold = 14 V */
    .lower_nonrecover_thr = (8000 >> 6), /* Lower non-recoverable Threshold = 8 V */
    .lower_critical_thr = (9000 >> 6), /* Lower critical Threshold = 9 V */
    .lower_noncritical_thr = (10000 >> 6), /* Lower non-critical Threshold = 10 V */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = MAX11609_CHANNEL_7, /* ADC channel 7 */
    .IDtypelen = 0xc0 | STR_SIZE("12V_HP"), /* 8 bit ASCII, number of bytes */
    .IDstring = "12V_HP" /*  sensor string */
};
#endif

void rtm_sdr_init( void )
{
#ifdef MODULE_HOTSWAP
    sdr_insert_entry( TYPE_02, (void *) &SDR_HOTSWAP_RTM, &vTaskHotSwap_Handle, 0, 0 );
#endif

#ifdef MODULE_LM75
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_RTM_1, &vTaskLM75_Handle, 0, CHIP_ID_RTM_LM75_0 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_RTM_2, &vTaskLM75_Handle, 0, CHIP_ID_RTM_LM75_1 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_RTM_3, &vTaskLM75_Handle, 0, CHIP_ID_RTM_LM75_2 );
#endif

#ifdef MODULE_MAX11609
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_VS1, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_VS2, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_N7V, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_7V, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_2V5, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_3V3, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_5V, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
    sdr_insert_entry(TYPE_01, (void *) &SDR_MAX11609_12V_HP, &vTask11609_Handle, 0, CHIP_ID_RTM_MAX11609);
#endif

}
