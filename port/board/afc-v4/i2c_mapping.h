#ifndef I2C_MAPPING_H_
#define I2C_MAPPING_H_

#include "i2c.h"

#define I2CMODE_POOLING         1
#define I2CMODE_INTERRUPT       0
#define SPEED_100KHZ            100000

enum {
    I2C_BUS_UNKNOWN_ID = 0x00,
    I2C_BUS_TEMP_SENSORS_ID,
    I2C_BUS_RTCE_ID,
    I2C_BUS_PORT2_ID,
    I2C_BUS_POWER_ID,
    I2C_BUS_CLOCK_ID,
    I2C_BUS_RTM_ID,
    I2C_BUS_FMC2_ID,
    I2C_BUS_FMC1_ID,
    I2C_BUS_MUX_ID,
    I2C_BUS_MCP_ID,
    I2C_BUS_CNT
};

enum {
    CHIP_ID_MUX = 0,
    CHIP_ID_MAX6642,
    CHIP_ID_LM75AIM_0,
    CHIP_ID_LM75AIM_1,
    CHIP_ID_LM75AIM_2,
    CHIP_ID_LM75AIM_3,
    CHIP_ID_RTC_EEPROM,
    CHIP_ID_SRAM_RTCC,
    CHIP_ID_EEPROM,
    CHIP_ID_EUI,
    CHIP_ID_EEPROM_64K,
    CHIP_ID_INA_0,
    CHIP_ID_INA_1,
    CHIP_ID_INA_2,
    CHIP_ID_8V54816,
    CHIP_ID_FMC2_EEPROM,
    CHIP_ID_FMC2_LM75_0,
    CHIP_ID_FMC2_LM75_1,
    CHIP_ID_FMC1_EEPROM,
    CHIP_ID_FMC1_LM75_0,
    CHIP_ID_FMC1_LM75_1,
    CHIP_ID_MCP23016,
    I2C_CHIP_CNT
};

#define I2C_MUX_CNT    2

extern i2c_mux_state_t i2c_mux[I2C_MUX_CNT];
extern i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT];
extern i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT];

#endif
