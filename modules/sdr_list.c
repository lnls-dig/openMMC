#include "sdr.h"
#include "fpga_spi.h"
#include "sensors.h"

/* Management Controller Device Locator Record 37.9 SDR Type 12h */
static SDR_type_12h_t  SDR0 = {
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
static SDR_type_02h_t SDR_HOT_SWAP = {
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
static SDR_type_01h_t SDR_FMC1_12V = {

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
static SDR_type_01h_t SDR_FMC1_VADJ = {

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
static SDR_type_01h_t SDR_FMC1_P3V3 = {

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
static SDR_type_01h_t SDR_FMC2_12V = {

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
static SDR_type_01h_t SDR_FMC2_VADJ = {

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
static SDR_type_01h_t SDR_FMC2_P3V3 = {

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

static SDR_type_01h_t SDR_LM75_1 = {

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

static SDR_type_01h_t SDR_LM75_2 = {

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

sensor_t sensor_array[NUM_SDR]  = {
    /* Entry record */
    { .type = TYPE_12, .sdr = (void *) &SDR0,          .sdr_length = sizeof(SDR0),          .task_handle = NULL,                 .diag_devID = NO_DIAG },
    { .type = TYPE_02, .sdr = (void *) &SDR_HOT_SWAP,  .sdr_length = sizeof(SDR_HOT_SWAP),  .task_handle = &vTaskHotSwap_Handle, .diag_devID = NO_DIAG },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC1_12V,  .sdr_length = sizeof(SDR_FMC1_12V),  .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC1_12V_DEVID,  .slave_addr = 0x40 },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC1_VADJ, .sdr_length = sizeof(SDR_FMC1_VADJ), .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC1_VADJ_DEVID, .slave_addr = 0x41 },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC1_P3V3, .sdr_length = sizeof(SDR_FMC1_P3V3), .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC1_P3V3_DEVID, .slave_addr = 0x43 },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC2_12V,  .sdr_length = sizeof(SDR_FMC2_12V),  .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC2_12V_DEVID,  .slave_addr = 0x45 },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC2_VADJ, .sdr_length = sizeof(SDR_FMC2_VADJ), .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC2_VADJ_DEVID, .slave_addr = 0x42 },
    { .type = TYPE_01, .sdr = (void *) &SDR_FMC2_P3V3, .sdr_length = sizeof(SDR_FMC2_P3V3), .task_handle = &vTaskINA220_Handle,  .diag_devID = FMC2_P3V3_DEVID, .slave_addr = 0x44 },
    { .type = TYPE_01, .sdr = (void *) &SDR_LM75_1,    .sdr_length = sizeof(SDR_LM75_1),    .task_handle = &vTaskLM75_Handle,    .diag_devID = NO_DIAG,        .slave_addr = 0x4C },
    { .type = TYPE_01, .sdr = (void *) &SDR_LM75_2,    .sdr_length = sizeof(SDR_LM75_2),    .task_handle = &vTaskLM75_Handle,    .diag_devID = NO_DIAG,        .slave_addr = 0x4D }
};

    /* Hotswap Sensor */
    /* FMC1 Current/Voltage/Power Sensors */
    /* FMC2 Current/Voltage/Power Sensors */
    /* Temperature Sensors */








