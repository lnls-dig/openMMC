/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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
#ifdef MODULE_INA3221_VOLTAGE
#include "ina3221.h"
#define SDR_AMC_12V_VOLTAGE_ID        "AMC +12V VOLTAGE"
#define SDR_FMC_12V_VOLTAGE_ID        "FMC +12V VOLTAGE"
#define SDR_RTM_12V_VOLTAGE_ID        "RTM +12V VOLTAGE"
#endif

#ifdef MODULE_INA3221_CURRENT
#include "ina3221.h"
#define SDR_AMC_12V_CURRENT_ID        "AMC +12V CURRENT"
#define SDR_FMC_12V_CURRENT_ID        "FMC +12V CURRENT"
#define SDR_RTM_12V_CURRENT_ID        "RTM +12V CURRENT"
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
    .IDtypelen = 0xc0 | STR_SIZE("HOTSWAP AMC"), /* 8 bit ASCII, number of bytes */
    .IDstring = "HOTSWAP AMC" /* sensor string */
};

#endif

#ifdef MODULE_INA3221_VOLTAGE

/* AMC 12V */
const SDR_type_01h_t SDR_AMC_12V_VOLTAGE = {

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
    .nominal_reading = (12000 >> 6), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] */
    .normal_max = (12500 >> 6), /* Normal maximum */
    .normal_min = (11500 >> 6), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (13000 >> 6), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (12800 >> 6), /* Upper critical Threshold */
    .upper_noncritical_thr = (12500 >> 6), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (11000 >> 6), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (11400 >> 6), /* Lower critical Threshold */
    .lower_noncritical_thr = (11500 >> 6), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_1, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_AMC_12V_VOLTAGE_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_AMC_12V_VOLTAGE_ID /* sensor string */
};

/* FMC 12V */
const SDR_type_01h_t SDR_FMC_12V_VOLTAGE = {

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
    .nominal_reading = (12000 >> 6), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] */
    .normal_max = (12500 >> 6), /* Normal maximum */
    .normal_min = (11500 >> 6), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (13000 >> 6), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (12800 >> 6), /* Upper critical Threshold */
    .upper_noncritical_thr = (12500 >> 6), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (11000 >> 6), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (11400 >> 6), /* Lower critical Threshold */
    .lower_noncritical_thr = (11500 >> 6), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_2, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC_12V_VOLTAGE_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC_12V_VOLTAGE_ID /* sensor string */
};

/* RTM 12V */
const SDR_type_01h_t SDR_RTM_12V_VOLTAGE = {

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
    .nominal_reading = (12000 >> 6), /* Nominal reading [(M * x + B * 10^(B_exp)) * 10^(R_exp)] */
    .normal_max = (12500 >> 6), /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (13000 >> 6), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (12800 >> 6), /* Upper critical Threshold */
    .upper_noncritical_thr = (12500 >> 6), /* Upper non critical Threshold */
    .lower_nonrecover_thr = 0, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 0, /* Lower critical Threshold */
    .lower_noncritical_thr = 0, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_3, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_RTM_12V_VOLTAGE_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_RTM_12V_VOLTAGE_ID /* sensor string */
};

#endif

#ifdef MODULE_INA3221_CURRENT

/* AMC 12V Current */
const SDR_type_01h_t SDR_AMC_12V_CURRENT = {

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
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm */
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: Current */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x0A80, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A80, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = (3000 >> 6), /* Nominal reading */
    .normal_max = (7000 >> 6), /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (8000 >> 6), /* Upper non-recoverable Threshold - 8 A */
    .upper_critical_thr = (7500 >> 6), /* Upper critical Threshold - 7.5 A */
    .upper_noncritical_thr = (7000 >> 6), /* Upper non critical Threshold - 7 A */
    .lower_nonrecover_thr = -15, /* Lower non-recoverable Threshold - -0.1 A */
    .lower_critical_thr = -10, /* Lower critical Threshold - -0.64 A */
    .lower_noncritical_thr = -5, /* Lower non-critical Threshold - -0.32 A */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_1, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_AMC_12V_CURRENT_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_AMC_12V_CURRENT_ID /* sensor string */
};

/* FMC 12V Current */
const SDR_type_01h_t SDR_FMC_12V_CURRENT = {

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
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm */
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: Current */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x0A80, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A80, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 15, /* Nominal reading */
    .normal_max = (2000 >> 6), /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (2100 >> 6), /* Upper non-recoverable Threshold - 2.1 A */
    .upper_critical_thr = (2000 >> 6), /* Upper critical Threshold - 2 A */
    .upper_noncritical_thr = (1800 >> 6), /* Upper non critical Threshold - 1.8 A */
    .lower_nonrecover_thr = -15, /* Lower non-recoverable Threshold - -0.1 A */
    .lower_critical_thr = -10, /* Lower critical Threshold - -0.64 A */
    .lower_noncritical_thr = -5, /* Lower non-critical Threshold - -0.32 A */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_2, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_FMC_12V_CURRENT_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_FMC_12V_CURRENT_ID /* sensor string */
};

/* RTM 12V Current */
const SDR_type_01h_t SDR_RTM_12V_CURRENT = {

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
    .sensorinit = 0x7F, /* init: event generation + scanning enabled */
    .sensorcap = 0x56, /* capabilities: auto re-arm */
    .sensortype = SENSOR_TYPE_CURRENT, /* sensor type: Current */
    .event_reading_type = 0x01, /* sensor reading*/
    .assertion_event_mask = 0x0A80, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A80, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x00, /* MSB: setable Threshold mask: none of the thresholds are setable: */
    .sensor_units_1 = 0x80, /* sensor units 1 :*/
    .sensor_units_2 = 0x05, /* sensor units 2 :*/
    .sensor_units_3 = 0x00, /* sensor units 3 :*/
    .linearization = 0x00, /* Linearization */
    .M = 64, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xD0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 30, /* Nominal reading */
    .normal_max = (5000 >> 6), /* Normal maximum */
    .normal_min = 0, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (5500 >> 6), /* Upper non-recoverable Threshold - 5.5 A */
    .upper_critical_thr = (5000 >> 6), /* Upper critical Threshold - 5 A */
    .upper_noncritical_thr = (4500 >> 6), /* Upper non critical Threshold - 4.5 A */
    .lower_nonrecover_thr = -15, /* Lower non-recoverable Threshold - -0.1 A */
    .lower_critical_thr = -10, /* Lower critical Threshold - -0.64 A */
    .lower_noncritical_thr = -5, /* Lower non-critical Threshold - -0.32 A */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = INA3221_CHANNEL_3, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE(SDR_RTM_12V_CURRENT_ID) , /* 8 bit ASCII, number of bytes */
    .IDstring = SDR_RTM_12V_CURRENT_ID /* sensor string */
};

#endif

#ifdef MODULE_LM75

/* LM75 SDR List */
const SDR_type_01h_t SDR_LM75_IC6 = {

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
    .nominal_reading = (20 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (10 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (80 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (75 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (65 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (0 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (5 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (10 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP IC6"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP IC6" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_IC7 = {

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
    .nominal_reading = (20 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (10 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (80 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (75 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (65 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (0 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (5 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (10 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP IC7"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP IC7" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_IC8 = {

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
    .nominal_reading = (20 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (10 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (80 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (75 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (65 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (0 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (5 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (10 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP IC8"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP IC8" /*  sensor string */
};

const SDR_type_01h_t SDR_LM75_IC9 = {

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
    .nominal_reading = (20 << 1), /* Nominal reading */
    .normal_max = (50 << 1), /* Normal maximum */
    .normal_min = (10 << 1), /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = (80 << 1), /* Upper non-recoverable Threshold */
    .upper_critical_thr = (75 << 1), /* Upper critical Threshold */
    .upper_noncritical_thr = (65 << 1), /* Upper non critical Threshold */
    .lower_nonrecover_thr = (0 << 1), /* Lower non-recoverable Threshold */
    .lower_critical_thr = (5 << 1), /* Lower critical Threshold */
    .lower_noncritical_thr = (10 << 1), /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP IC9"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP IC9" /* sensor string */
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
    .assertion_event_mask = 0x7A95, /* assertion event mask (All upper going-high and lower going-low events) */
    .deassertion_event_mask = 0x7A95, /* deassertion event mask (All upper going-high and lower going-low events) */
    .readable_threshold_mask = 0x3F, /* LSB: readable Threshold mask: all thresholds are readable:  */
    .settable_threshold_mask = 0x0, /* MSB: setable Threshold mask: none of the thresholds are setable: */
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
    .nominal_reading = 20, /* Nominal reading */
    .normal_max = 50, /* Normal maximum */
    .normal_min = 10, /* Normal minimum */
    .sensor_max_reading = 0xFF, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 80, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 75, /* Upper critical Threshold */
    .upper_noncritical_thr = 65, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 0, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 5, /* Lower critical Threshold */
    .lower_noncritical_thr = 10, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | STR_SIZE("TEMP FPGA"), /* 8 bit ASCII, number of bytes */
    .IDstring = "TEMP FPGA" /*  sensor string */
};
#endif

#if defined(MODULE_INA3221_VOLTAGE) || defined(MODULE_INA3221_CURRENT)

// INA3221 configuration
const ina3221_config_t INA3221_SETTINGS = {
    .shunt_resistor[0] = 10, /* mOhm */
    .shunt_resistor[1] = 20, /* mOhm */
    .shunt_resistor[2] = 20  /* mOhm */
};

#endif

void amc_sdr_init( void )
{
    /* Hotswap Sensor */
    sdr_insert_entry( TYPE_02, (void *) &SDR_HOTSWAP_AMC, &vTaskHotSwap_Handle, 0, 0 );

    /* INA3221 sensors */
#ifdef MODULE_INA3221_VOLTAGE

    sdr_insert_entry( TYPE_01, (void *) &SDR_AMC_12V_VOLTAGE, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC_12V_VOLTAGE, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_RTM_12V_VOLTAGE, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_add_settings(CHIP_ID_INA3221, (void *) &INA3221_SETTINGS);

#endif

#ifdef MODULE_INA3221_CURRENT

    sdr_insert_entry( TYPE_01, (void *) &SDR_AMC_12V_CURRENT, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_FMC_12V_CURRENT, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_RTM_12V_CURRENT, &vTaskINA3221_Handle, 0, CHIP_ID_INA3221 );
    sdr_add_settings(CHIP_ID_INA3221, (void *) &INA3221_SETTINGS);

#endif

    /* FPGA Temperature */
#ifdef MODULE_MAX6642

    sdr_insert_entry( TYPE_01, (void *) &SDR_MAX6642_FPGA, &vTaskMAX6642_Handle, 0, CHIP_ID_MAX6642 );

#endif

    /* Board Temperature */
#ifdef MODULE_LM75

    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_IC6, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_0 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_IC7, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_1 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_IC8, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_2 );
    sdr_insert_entry( TYPE_01, (void *) &SDR_LM75_IC9, &vTaskLM75_Handle, 0, CHIP_ID_LM75AIM_3 );

#endif
}
