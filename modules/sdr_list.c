#include "sdr.h"
#include "fpga_spi.h"
#include "sdr_list.h"
#include "sensors.h"

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
    .power_notification_global_init = 0x04,
    .device_cap = 0x3b,
    .reserved[0] = 0x00,
    .reserved[1] = 0x00,
    .reserved[2] = 0x00,
    .entityID = 0xC1,
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
    .IDtypelen = 0xcF, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'M', 'O', 'D', 'U', 'L', 'E', ' ', 'H', 'O', 'T', '_', 'S', 'W', 'A', 'P' } /* sensor string */
};

/* 12V sensor */
static const SDR_type_01h_t SDR_FMC1_12V = {

    .hdr.recID_LSB = NUM_SDR_FMC1_12V,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC1_12V, /* sensor number */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 195, /* Nominal reading = 12.285V */
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
    .IDtypelen = 0xc0 | 9 , /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','1',' ','+', '1', '2', 'V' } /* sensor string */
};

/* FMC2 PVADJ sensor */
static const SDR_type_01h_t SDR_FMC1_VADJ = {

    .hdr.recID_LSB = NUM_SDR_FMC1_VADJ,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC1_VADJ, /* sensor number */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: voltage*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
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
    .IDstring = { 'F','M','C','1',' ', 'V', 'A', 'D', 'J' } /* sensor string */
};

/* FMC2 PVADJ sensor */
static const SDR_type_01h_t SDR_FMC1_P3V3 = {

    .hdr.recID_LSB = NUM_SDR_FMC1_3V3,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_FMC1_3V3, /* sensor number */

    /* record body bytes */
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: VOLTAGE*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 33, /* Nominal reading */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 40, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 38, /* Upper critical Threshold */
    .upper_noncritical_thr = 36, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 26, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 28, /* Lower critical Threshold */
    .lower_noncritical_thr = 30, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 9, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','1',' ', '+', '3', 'V', '3' } /* sensor string */
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
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x02, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp = -3 , B-Exp = 0 */
    .analog_flags = 0x03, /* Analogue characteristics flags */
    .nominal_reading = 125, /* Nominal reading = 12.285V */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 250, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 130, /* Upper critical Threshold */
    .upper_noncritical_thr = 128, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 100, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 110, /* Lower critical Threshold */
    .lower_noncritical_thr = 115, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 9 , /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','2',' ','+', '1', '2', 'V' } /* sensor string */
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
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: voltage*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
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
    .entityID = 0xC1, /* entity id: AMC Module */
    .entityinstance = 0x00, /* entity instance -> SDR_Init */
    .sensorinit = 0x7f, /* init: event generation + scanning enabled */
    .sensorcap = 0x58, /* capabilities: auto re-arm,*/
    .sensortype = SENSOR_TYPE_VOLTAGE, /* sensor type: VOLTAGE*/
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
    .M = 1, /* M */
    .M_tol = 0x00, /* M - Tolerance */
    .B = 0x00, /* B */
    .B_accuracy = 0x00, /* B - Accuracy */
    .acc_exp_sensor_dir = 0x00, /* Sensor direction */
    .Rexp_Bexp = 0xF0, /* R-Exp , B-Exp */
    .analog_flags = 0x00, /* Analogue characteristics flags */
    .nominal_reading = 33, /* Nominal reading */
    .normal_max = 255, /* Normal maximum */
    .normal_min = 0x00, /* Normal minimum */
    .sensor_max_reading = 255, /* Sensor Maximum reading */
    .sensor_min_reading = 0x00, /* Sensor Minimum reading */
    .upper_nonrecover_thr = 40, /* Upper non-recoverable Threshold */
    .upper_critical_thr = 38, /* Upper critical Threshold */
    .upper_noncritical_thr = 36, /* Upper non critical Threshold */
    .lower_nonrecover_thr = 26, /* Lower non-recoverable Threshold */
    .lower_critical_thr = 28, /* Lower critical Threshold */
    .lower_noncritical_thr = 30, /* Lower non-critical Threshold */
    .pos_thr_hysteresis = 2, /* positive going Threshold hysteresis value */
    .neg_thr_hysteresis = 2, /* negative going Threshold hysteresis value */
    .reserved1 = 0x00, /* reserved */
    .reserved2 = 0x00, /* reserved */
    .OEM = 0x00, /* OEM reserved */
    .IDtypelen = 0xc0 | 9, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'F','M','C','2',' ', '+', '3', 'V', '3' } /* sensor string */
};

static const SDR_type_01h_t SDR_LM75_1 = {

    .hdr.recID_LSB = NUM_SDR_LM75_1,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_LM75_1, /* sensor number */

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
    .IDtypelen = 0xc0 | 6, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'T','E','M','P',' ', '1' } /*  sensor string */
};

static const SDR_type_01h_t SDR_LM75_2 = {

    .hdr.recID_LSB = NUM_SDR_LM75_2,
    .hdr.recID_MSB = 0x00,
    .hdr.SDRversion = 0x51,
    .hdr.rectype = TYPE_01,
    .hdr.reclength = sizeof(SDR_type_01h_t) - sizeof(SDR_entry_hdr_t),

    .ownerID = 0x00, /* i2c address, -> SDR_Init */
    .ownerLUN = 0x00, /* sensor owner LUN */
    .sensornum = NUM_SDR_LM75_2, /* sensor number */

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
    .IDtypelen = 0xc0 | 6, /* 8 bit ASCII, number of bytes */
    .IDstring = { 'T','E','M','P',' ', '2' } /*  sensor string */
};

const sensor_t const sensor_array[NUM_SDR]  = {
    /* Entry record */
    { TYPE_12, (void *) &SDR0,          sizeof(SDR0),          &sdrData[0], (TaskHandle_t *) NULL,                 0   , NO_DIAG },
    /* Hotswap Sensor */
    { TYPE_02, (void *) &SDR_HOT_SWAP,  sizeof(SDR_HOT_SWAP),  &sdrData[1], (TaskHandle_t *) &vTaskHotSwap_Handle, 0   , NO_DIAG },
    /* FMC1 Current/Voltage/Power Sensors */
    { TYPE_01, (void *) &SDR_FMC1_12V,  sizeof(SDR_FMC1_12V),  &sdrData[2], (TaskHandle_t *) &vTaskINA220_Handle,  0x40, FMC1_12V_DEVID },
    { TYPE_01, (void *) &SDR_FMC1_VADJ, sizeof(SDR_FMC1_VADJ), &sdrData[3], (TaskHandle_t *) &vTaskINA220_Handle,  0x41, FMC1_VADJ_DEVID },
    { TYPE_01, (void *) &SDR_FMC1_P3V3, sizeof(SDR_FMC1_P3V3), &sdrData[4], (TaskHandle_t *) &vTaskINA220_Handle,  0x43, FMC1_P3V3_DEVID },
    /* FMC2 Current/Voltage/Power Sensors */
    { TYPE_01, (void *) &SDR_FMC2_12V,  sizeof(SDR_FMC2_12V),  &sdrData[5], (TaskHandle_t *) &vTaskINA220_Handle,  0x45, FMC2_12V_DEVID },
    { TYPE_01, (void *) &SDR_FMC2_VADJ, sizeof(SDR_FMC2_VADJ), &sdrData[6], (TaskHandle_t *) &vTaskINA220_Handle,  0x42, FMC2_VADJ_DEVID },
    { TYPE_01, (void *) &SDR_FMC2_P3V3, sizeof(SDR_FMC2_P3V3), &sdrData[7], (TaskHandle_t *) &vTaskINA220_Handle,  0x44, FMC2_P3V3_DEVID },
    /* Temperature Sensors */
    { TYPE_01, (void *) &SDR_LM75_1,    sizeof(SDR_LM75_1),    &sdrData[8], (TaskHandle_t *) &vTaskLM75_Handle,    0x4C, NO_DIAG },
    { TYPE_01, (void *) &SDR_LM75_2,    sizeof(SDR_LM75_2),    &sdrData[9], (TaskHandle_t *) &vTaskLM75_Handle,    0x4D, NO_DIAG }
};

