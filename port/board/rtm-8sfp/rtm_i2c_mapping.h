#ifndef RTM_I2C_MAPPING_H_
#define RTM_I2C_MAPPING_H_

#include "i2c.h"

enum {
    CHIP_ID_RTM_PCA9554 = 64,
    CHIP_ID_RTM_EEPROM,
    CHIP_ID_RTM_LM75_0,
    CHIP_ID_RTM_LM75_1
};

#define I2C_CHIP_RTM_CNT   4

extern i2c_chip_mapping_t i2c_chip_rtm_map[I2C_CHIP_RTM_CNT];

#endif
