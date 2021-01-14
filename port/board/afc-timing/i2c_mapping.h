#ifndef I2C_MAPPING_H_
#define I2C_MAPPING_H_

#include "i2c.h"

#define I2CMODE_POOLING         1
#define I2CMODE_INTERRUPT       0
#define SPEED_100KHZ            100000

// BUS_ID
// 0 - FMC1
// 1 - FMC2
// 3 - CPU_ID
//
///////////////////////

enum {
    CARRIER_TYPE_AFC = 0x01,
    CARRIER_TYPE_AFCK,
    CARRIER_TYPE_UNKNOWN = 0xFF
};

enum {
    I2C_BUS_UNKNOWN_ID = 0x00,
    I2C_BUS_FMC1_ID,
    I2C_BUS_FMC2_ID,
    I2C_BUS_CPU_ID,
    I2C_BUS_RTM_ID,
    I2C_BUS_CLOCK_ID,
    I2C_BUS_FPGA_ID
};

enum {
    CHIP_ID_MUX = 0,
    CHIP_ID_LM75AIM_0,
    CHIP_ID_LM75AIM_1,
    CHIP_ID_LM75AIM_2,
    CHIP_ID_LM75AIM_3,
    CHIP_ID_MAX6642,
    CHIP_ID_RTC,
    CHIP_ID_RTC_EEPROM,
    CHIP_ID_EEPROM,
    CHIP_ID_EEPROM_ID,
    CHIP_ID_INA_0,
    CHIP_ID_INA_1,
    CHIP_ID_INA_2,
    CHIP_ID_INA_3,
    CHIP_ID_INA_4,
    CHIP_ID_INA_5,
    CHIP_ID_ADN,
    CHIP_ID_SI57x,
    CHIP_ID_FMC1_EEPROM,
    CHIP_ID_FMC1_LM75_1,
    CHIP_ID_FMC1_LM75_0,
    CHIP_ID_FMC2_EEPROM,
    CHIP_ID_FMC2_LM75_1,
    CHIP_ID_FMC2_LM75_0,
    CHIP_ID_RTM_PCA9554,
    CHIP_ID_RTM_EEPROM,
    CHIP_ID_RTM_LM75_0,
    CHIP_ID_RTM_LM75_1
};

#define I2C_MUX_CNT    2
#define I2C_BUS_CNT    7
#define I2C_CHIP_CNT   28

extern i2c_mux_state_t i2c_mux[I2C_MUX_CNT];
extern i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT];
extern i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT];

#endif
