#ifndef I2C_MAPPING_H_
#define I2C_MAPPING_H_

#include "i2c.h"

#define I2CMODE_POOLING         1
#define I2CMODE_INTERRUPT       0
#define SPEED_100KHZ            100000

enum {
    I2C_BUS_UNKNOWN_ID = 0x00,
    I2C_BUS_FMC1_ID,
    I2C_BUS_FMC2_ID,
    I2C_BUS_CLOCKS_ID,
    I2C_BUS_EXAR_ID,
    I2C_BUS_PM_ID,
    I2C_BUS_SENS_ID,
    I2C_BUS_RTM_ID,
    I2C_BUS_PM_GPIO_ID,
    I2C_BUS_CPU_ID,
    I2C_BUS_FPGA_ID,
    I2C_BUS_CNT
};

enum {
    /* Chips on AMC */
    CHIP_ID_MUX_MMC = 0,
    CHIP_ID_MUX_FPGA,
    CHIP_ID_FMC1_EEPROM,
    CHIP_ID_FMC2_EEPROM,
    CHIP_ID_SI5341,
    CHIP_ID_TCA9539_CLK,
    CHIP_ID_8V54816,
    CHIP_ID_XR7724_1,
    CHIP_ID_XR7724_2,
    CHIP_ID_INA3221,
    CHIP_ID_LM75AIM_0,
    CHIP_ID_LM75AIM_1,
    CHIP_ID_LM75AIM_2,
    CHIP_ID_LM75AIM_3,
    CHIP_ID_MAX6642,
    CHIP_ID_EEPROM,
    CHIP_ID_EEPROM_ID,
    CHIP_ID_EEPROM_64K,
#ifdef MODULE_RTM
    CHIP_ID_RTM_LM75_0,
    CHIP_ID_RTM_LM75_1,
    CHIP_ID_RTM_LM75_2,
    CHIP_ID_RTM_PCA9554,
    CHIP_ID_RTM_EEPROM,
    CHIP_ID_RTM_EEPROM_ID,
#endif
    CHIP_ID_TCA9539_PM,
    I2C_CHIP_CNT
};

#define I2C_MUX_CNT    2

extern i2c_mux_state_t i2c_mux[I2C_MUX_CNT];
extern i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT];
extern i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT];

#endif
