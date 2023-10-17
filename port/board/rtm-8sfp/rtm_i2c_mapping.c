#include "i2c.h"
#include "rtm_i2c_mapping.h"
#include "i2c_mapping.h"
#include "port.h"

i2c_chip_mapping_t i2c_chip_rtm_map[I2C_CHIP_RTM_CNT] = {
    [CHIP_ID_RTM_PCA9554 - 64] = { I2C_BUS_RTM_ID,     0x20 },
    [CHIP_ID_RTM_EEPROM - 64]  = { I2C_BUS_RTM_ID,     0x50 },
    [CHIP_ID_RTM_LM75_0 - 64]  = { I2C_BUS_RTM_ID,     0x48 },
    [CHIP_ID_RTM_LM75_1 - 64]  = { I2C_BUS_RTM_ID,     0x49 },
};
