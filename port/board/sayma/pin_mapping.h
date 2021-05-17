/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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

/**
 * @defgroup SAYMA Sayma Board Port
 * @ingroup BOARD_PORTS
 */

/**
 * @file sayma/pin_mapping.h
 * @brief Hardware pin definitions for Sayma
 *
 * @ingroup SAYMA_PIN_MAPPING
 */

/**
 * @defgroup SAYMA_PIN_MAPPING Sayma Pin Mapping
 * @ingroup Sayma
 * @{
 */

#ifndef PIN_MAPPING_H_
#define PIN_MAPPING_H_

#include "../../../modules/pin_cfg.h"

/* SPI Interfaces */
#define FLASH_SPI                       0
#define FPGA_SPI                        1

/* UART Interfaces */
#define MMC_UART1                       1
#define MMC_UART4                       4
#define UART_DEBUG                      3

/* Pin definitions */
#define BOOT_MODE0                      PIN_DEF( PORT2, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define BOOT_MODE1                      PIN_DEF( PORT2, 17, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define BOOT_MODE2                      PIN_DEF( PORT2, 18, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define BOOT_MODE3                      PIN_DEF( PORT2, 19, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

#define LPC_ISP_SWn                     PIN_DEF( PORT2, 10, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* I2C ports */
#define I2C0_SDA                        PIN_DEF( PORT0, 27, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C0_SCL                        PIN_DEF( PORT0, 28, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C1_SDA                        PIN_DEF( PORT0,  0, (IOCON_FUNC3 | IOCON_OPENDRAIN_EN), NON_GPIO )
#define I2C1_SCL                        PIN_DEF( PORT0,  1, (IOCON_FUNC3 | IOCON_OPENDRAIN_EN), NON_GPIO )
#define I2C2_SDA                        PIN_DEF( PORT0, 10, (IOCON_FUNC2 | IOCON_OPENDRAIN_EN), NON_GPIO )
#define I2C2_SCL                        PIN_DEF( PORT0, 11, (IOCON_FUNC2 | IOCON_OPENDRAIN_EN), NON_GPIO )

#define GPIO_I2C_MUX_ADDR1              PIN_DEF( PORT2, 29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_I2C_MUX_ADDR2              PIN_DEF( PORT2, 28, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_SW_RESETn                  PIN_DEF( PORT5, 0, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* UART Debug port */
#define UART_DEBUG_TXD                  PIN_DEF( PORT0, 2, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_RXD                  PIN_DEF( PORT0, 3, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_PORT                 0
#define UART_DEBUG_TXD_PIN              2
#define UART_DEBUG_RXD_PIN              3

/* SSP Ports */
/* Flash CPU SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP0_SCK                        PIN_DEF( PORT0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_SSEL                       PIN_DEF( PORT0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP0_MISO                       PIN_DEF( PORT0, 17, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_MOSI                       PIN_DEF( PORT0, 18, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
/* FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP1_SCK                        PIN_DEF( PORT0,  7, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_SSEL                       PIN_DEF( PORT0,  6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP1_MISO                       PIN_DEF( PORT0,  8, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_MOSI                       PIN_DEF( PORT0,  9, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
/* Flash FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define FLASH_UPD_CLK                   PIN_DEF( PORT1, 20, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define FLASH_UPD_CSB                   PIN_DEF( PORT1, 21, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define FLASH_UPD_MISO                  PIN_DEF( PORT1, 23, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define FLASH_UPD_MOSI                  PIN_DEF( PORT1, 24, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )

/* GPIO definitions */
/* Geographic Address pin definitions */
#define GPIO_GA0                        PIN_DEF( PORT3,  0, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA1                        PIN_DEF( PORT3,  1, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA2                        PIN_DEF( PORT3,  2, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA_TEST                    PIN_DEF( PORT3,  3, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* Board LEDs */
#define GPIO_LEDBLUE                    PIN_DEF( PORT3, 4, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_LEDGREEN                   PIN_DEF( PORT3, 5, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_LEDRED                     PIN_DEF( PORT1, 25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* Front Panel BUTTON */
#define GPIO_FRONT_BUTTON               PIN_DEF( PORT4, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Hot swap handle */
#define GPIO_HOT_SWAP_HANDLE            PIN_DEF( PORT4, 13, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* DCDC converters */
/* NOTE: Due to a hardware limitation, pins 29 and 30 from port 0 will have the same direction, even if set differently in the register */
#define GPIO_P12V0_OK                   PIN_DEF( PORT0, 30, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_EN_FMC1_P12V               PIN_DEF( PORT1, 27, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P0V9                    PIN_DEF( PORT1, 28, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT ) // EN_VCCINT
#define GPIO_EN_P5V0                    PIN_DEF( PORT1, 29, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_PSU_CH                  PIN_DEF( PORT1, 31, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_PGOOD_P5V0                 PIN_DEF( PORT1, 22, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_SDRAM_PGOOD                PIN_DEF( PORT2, 4, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )
#define GPIO_P3V3_FMC_PGOOD             PIN_DEF( PORT1, 6, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_EN_P3V3_FMC                PIN_DEF( PORT1, 7, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P3V3_CLK                PIN_DEF( PORT1, 8, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
/* LTM Control */
#define GPIO_EN_1V2                     PIN_DEF( PORT1, 10, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P0V95                   PIN_DEF( PORT1, 11, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_1V0                     PIN_DEF( PORT1, 12, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_3V3                     PIN_DEF( PORT1, 13, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_1V8                     PIN_DEF( PORT1, 14, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_1V5                     PIN_DEF( PORT1, 15, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_LTM_PGOOD                  PIN_DEF( PORT1, 16, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_P0V95_PG                   PIN_DEF( PORT1, 17, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_P1V2_PG                    PIN_DEF( PORT1, 18, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_P0V95I_PG                  PIN_DEF( PORT1, 19, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )

/* RTM */
#define GPIO_EN_RTM_MP                  PIN_DEF( PORT1, 30, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_RTM_PWR                 PIN_DEF( PORT1, 26, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
/* NOTE: Due to a hardware limitation, pins 29 and 30 from port 0 will have the same direction, even if set differently in the register */
#define GPIO_RTM_PS                     PIN_DEF( PORT0, 29, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* FPGA Control */
#define GPIO_FPGA_PROGRAM_B             PIN_DEF( PORT0, 4, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_DONE_B                PIN_DEF( PORT0, 5, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_RESETn                PIN_DEF( PORT4, 9, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_INITB                 PIN_DEF( PORT0, 13, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_STATUS                PIN_DEF( PORT0, 14, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_FLASH_UPDATE            PIN_DEF( PORT0, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )


#define GPIO_RTM_JTAG_Override          PIN_DEF( PORT4,  25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_FMC_JTAG_Override          PIN_DEF( PORT4,  26, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* MMC ENABLE# */
#define GPIO_MMC_ENABLE                 PIN_DEF( PORT4,  8, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* Overtemp signal */
#define GPIO_OVERTEMPn                  PIN_DEF( PORT4, 11, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* FMC Present signals */
#define GPIO_FMC1_PRSNT_M2C             PIN_DEF( PORT3, 6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FMC Power Good signals */
#define GPIO_FMC1_PG_M2C                PIN_DEF( PORT3, 8, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )
#define GPIO_FMC1_PG_C2M                PIN_DEF( PORT3, 10, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )

#define GPIO_FMC1_DIR                   PIN_DEF( PORT3, 12, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )

/* Hardware ID */
#define GPIO_HW_ID0                     PIN_DEF( PORT3, 23, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID1                     PIN_DEF( PORT3, 24, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID2                     PIN_DEF( PORT3, 25, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID3                     PIN_DEF( PORT3, 26, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )


/* Pin initialization (config) list */
#define PIN_CFG_LIST                            \
        LPC_ISP_SWn,                            \
        BOOT_MODE0,                             \
        BOOT_MODE1,                             \
        BOOT_MODE2,                             \
        BOOT_MODE3,                             \
        I2C0_SDA,                               \
        I2C0_SCL,                               \
        I2C1_SDA,                               \
        I2C1_SCL,                               \
        I2C2_SDA,                               \
        I2C2_SCL,                               \
        GPIO_I2C_MUX_ADDR1,                     \
        GPIO_I2C_MUX_ADDR2,                     \
        GPIO_SW_RESETn,                         \
        UART_DEBUG_TXD,                         \
        UART_DEBUG_RXD,                         \
        GPIO_GA0,                               \
        GPIO_GA1,                               \
        GPIO_GA2,                               \
        GPIO_GA_TEST,                           \
        GPIO_LEDBLUE,                           \
        GPIO_LEDGREEN,                          \
        GPIO_LEDRED,                            \
        GPIO_FRONT_BUTTON,                      \
        GPIO_HOT_SWAP_HANDLE,                   \
        GPIO_P12V0_OK,                          \
        GPIO_EN_FMC1_P12V,                      \
        GPIO_EN_P0V9,                           \
        GPIO_EN_P5V0,                           \
        GPIO_EN_PSU_CH,                         \
        GPIO_PGOOD_P5V0,                        \
        GPIO_SDRAM_PGOOD,                       \
        GPIO_P3V3_FMC_PGOOD,                    \
        GPIO_EN_P3V3_FMC,                       \
        GPIO_EN_P3V3_CLK,                       \
        GPIO_EN_1V2,                            \
        GPIO_EN_P0V95,                          \
        GPIO_EN_1V0,                            \
        GPIO_EN_3V3,                            \
        GPIO_EN_1V8,                            \
        GPIO_EN_1V5,                            \
        GPIO_LTM_PGOOD,                         \
        GPIO_P0V95_PG,                          \
        GPIO_P1V2_PG,                           \
        GPIO_P0V95I_PG,                         \
        GPIO_EN_RTM_MP,                         \
        GPIO_EN_RTM_PWR,                        \
        GPIO_RTM_PS,                            \
        GPIO_MMC_ENABLE,                        \
        GPIO_OVERTEMPn,                         \
        GPIO_FMC1_PRSNT_M2C,                    \
        GPIO_FMC1_PG_M2C,                       \
        GPIO_FMC1_PG_C2M,                       \
        GPIO_FMC1_DIR,                          \
        GPIO_HW_ID0,                            \
        GPIO_HW_ID1,                            \
        GPIO_HW_ID2,                            \
        GPIO_HW_ID3,                            \
        GPIO_FPGA_PROGRAM_B,                    \
        GPIO_FPGA_DONE_B,                       \
        GPIO_FPGA_RESETn,                       \
        GPIO_FPGA_INITB,                        \
        GPIO_FPGA_STATUS,                       \
        GPIO_EN_FLASH_UPDATE,                   \
        GPIO_RTM_JTAG_Override,                 \
        GPIO_FMC_JTAG_Override,                 \
        SSP0_SCK,                               \
        SSP0_SSEL,                              \
        SSP0_MISO,                              \
        SSP0_MOSI,                              \
        SSP1_SCK,                               \
        SSP1_SSEL,                              \
        SSP1_MISO,                              \
        SSP1_MOSI,                              \
        FLASH_UPD_CLK,                          \
        FLASH_UPD_CSB,                          \
        FLASH_UPD_MISO,                         \
        FLASH_UPD_MOSI                          \

#endif
/**
 * @}
 */
