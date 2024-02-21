#include "pin_mapping.h"

/**
 * @defgroup AFC_V4_0_PIN_MAPPING AFCv4.0 Pin Mapping
 * @ingroup AFC_V4_0
 * @{
 */
const external_gpio_t ext_gpios[16] = {
        [EXT_GPIO_P1V5_VTT_EN] =     { 1, 7 },
        [EXT_GPIO_EN_P1V8] =         { 1, 6 },
        [EXT_GPIO_EN_P1V2] =         { 1, 5 },
        [EXT_GPIO_EN_FMC1_P12V] =    { 1, 4 },
        [EXT_GPIO_EN_FMC2_P12V] =    { 1, 3 },
        [EXT_GPIO_EN_FMC1_PVADJ] =   { 1, 2 },
        [EXT_GPIO_EN_FMC2_PVADJ] =   { 1, 1 },
        [EXT_GPIO_EN_FMC1_P3V3] =    { 1, 0 },
        [EXT_GPIO_EN_FMC2_P3V3] =    { 0, 7 },
        [EXT_GPIO_EN_P1V0] =         { 0, 6 },
        [EXT_GPIO_EN_P3V3] =         { 0, 5 },
        [EXT_GPIO_EN_RTM_PWR] =      { 0, 4 },
        [EXT_GPIO_EN_RTM_MP] =       { 0, 3 },
        [EXT_GPIO_FPGA_I2C_RESET] =  { 0, 2 },
        [EXT_GPIO_DAC_VADJ_RSTn] =   { 0, 1 },
        [EXT_GPIO_PROGRAM_B] =       { 0, 0 }
};
/**
 * @}
 */
