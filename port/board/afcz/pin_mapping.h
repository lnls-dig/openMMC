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
 * @defgroup AFCZ Board Port
 * @ingroup BOARD_PORTS
 */

/**
 * @file afcz/pin_mapping.h
 * @brief Hardware pin definitions for AFCZ
 *
 * @ingroup AFCZ_PIN_MAPPING
 */

/**
 * @defgroup AFCZ_PIN_MAPPING AFCZ Pin Mapping
 * @ingroup AFCZ
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

/* I2C ports */
#define I2C0_SDA                        PIN_DEF( PORT0, 27, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C0_SCL                        PIN_DEF( PORT0, 28, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )

#define I2C1_SDA                        PIN_DEF( PORT0,  0, (IOCON_FUNC3 | IOCON_OPENDRAIN_EN), NON_GPIO )
#define I2C1_SCL                        PIN_DEF( PORT0,  1, (IOCON_FUNC3 | IOCON_OPENDRAIN_EN), NON_GPIO )

#define I2C2_SDA                        PIN_DEF( PORT0, 10, (IOCON_FUNC2 | IOCON_OPENDRAIN_EN), NON_GPIO )
#define I2C2_SCL                        PIN_DEF( PORT0, 11, (IOCON_FUNC2 | IOCON_OPENDRAIN_EN), NON_GPIO )

/* UART ports */
#define MMC_UART1_TXD                   PIN_DEF( PORT2, 0, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define MMC_UART1_RXD                   PIN_DEF( PORT2, 1, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

#define MMC_UART4_TXD                   PIN_DEF( PORT5, 4, (IOCON_FUNC4 | IOCON_MODE_INACT), NON_GPIO )
#define MMC_UART4_RXD                   PIN_DEF( PORT5, 3, (IOCON_FUNC4 | IOCON_MODE_INACT), NON_GPIO )

#define UART_DEBUG_TXD                  PIN_DEF( PORT0, 2, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_RXD                  PIN_DEF( PORT0, 3, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_PORT                 0
#define UART_DEBUG_TXD_PIN              2
#define UART_DEBUG_RXD_PIN              3

/* SSP Ports */
/* FLASH CPU SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP0_SCK                        PIN_DEF( PORT0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_SSEL                       PIN_DEF( PORT0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP0_MISO                       PIN_DEF( PORT0, 17, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_MOSI                       PIN_DEF( PORT0, 18, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

/* FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP1_SCK                        PIN_DEF( PORT0,  7, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_SSEL                       PIN_DEF( PORT0,  6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP1_MISO                       PIN_DEF( PORT0,  8, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_MOSI                       PIN_DEF( PORT0,  9, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

/* FLASH FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define FLASH_UPD_CLK                   PIN_DEF( PORT1, 20, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define FLASH_UPD_CSB                   PIN_DEF( PORT1, 21, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define FLASH_UPD_MISO                  PIN_DEF( PORT1, 23, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define FLASH_UPD_MOSI                  PIN_DEF( PORT1, 24, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )

/* Ethernet Port */
#define ENET_PHY_MII1_TXD0              PIN_DEF( PORT1,  0, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_TXD1              PIN_DEF( PORT1,  1, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_TXD2              PIN_DEF( PORT1,  2, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_TXD3              PIN_DEF( PORT1,  3, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_TX_EN             PIN_DEF( PORT1,  4, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_TX_CLK            PIN_DEF( PORT1,  6, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_COL               PIN_DEF( PORT1,  7, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_CRS               PIN_DEF( PORT1,  8, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RXD0              PIN_DEF( PORT1,  9, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RXD1              PIN_DEF( PORT1,  10, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RXD2              PIN_DEF( PORT1,  11, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RXD3              PIN_DEF( PORT1,  12, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RX_DV             PIN_DEF( PORT1,  13, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RX_ER             PIN_DEF( PORT1,  14, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_RX_CLK            PIN_DEF( PORT1,  15, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )

#define ENET_PHY_MII1_MDC               PIN_DEF( PORT1,  16, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define ENET_PHY_MII1_MDIO              PIN_DEF( PORT1,  17, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )

/* TRACEDATA */
#define TRACEDATA3                      PIN_DEF( PORT2,  2, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA2                      PIN_DEF( PORT2,  3, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA1                      PIN_DEF( PORT2,  4, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA0                      PIN_DEF( PORT2,  5, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )
#define TRACECLK                        PIN_DEF( PORT2,  6, (IOCON_FUNC5 | IOCON_MODE_INACT), NON_GPIO )


/* GPIO definitions */

/* I2C MUX */
#define GPIO_I2C_MUX_ADDR1              PIN_DEF( PORT2, 29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_I2C_MUX_ADDR2              PIN_DEF( PORT2, 28, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_SW_RESETn                  PIN_DEF( PORT5, 0,  (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* BOOT MODE */
#define GPIO_BOOT_MODE0                 PIN_DEF( PORT2, 16, (IOCON_FUNC0 | IOCON_OPENDRAIN_EN), GPIO_DIR_OUTPUT )
#define GPIO_BOOT_MODE1                 PIN_DEF( PORT2, 17, (IOCON_FUNC0 | IOCON_OPENDRAIN_EN), GPIO_DIR_OUTPUT )
#define GPIO_BOOT_MODE2                 PIN_DEF( PORT2, 18, (IOCON_FUNC0 | IOCON_OPENDRAIN_EN), GPIO_DIR_OUTPUT )
#define GPIO_BOOT_MODE3                 PIN_DEF( PORT2, 19, (IOCON_FUNC0 | IOCON_OPENDRAIN_EN), GPIO_DIR_OUTPUT )

/* LPC ISP */
#define GPIO_LPC_ISP_SWn                PIN_DEF( PORT2, 10, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Geographic Address */
#define GPIO_GA0                        PIN_DEF( PORT3,  0, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA1                        PIN_DEF( PORT3,  1, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA2                        PIN_DEF( PORT3,  2, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA_TEST                    PIN_DEF( PORT3,  3, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* Board LEDs */
#define GPIO_LEDBLUE                    PIN_DEF( PORT3,  4, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_LEDGREEN                   PIN_DEF( PORT3,  5, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_LEDRED                     PIN_DEF( PORT1, 25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* Front panel button */
#define GPIO_FRONT_BUTTON               PIN_DEF( PORT4, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Hot swap handle */
#define GPIO_HOT_SWAP_HANDLE            PIN_DEF( PORT4, 13, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* DCDC converters */
/* NOTE: Due to a hardware limitation, pins 29 and 30 from port 0 will have the same direction, even if set differently in the register */
#define GPIO_P12V0_OK					PIN_DEF( PORT0, 30, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PGOOD_P5V0                 PIN_DEF( PORT1, 22, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_SDRAM_PGOOD                PIN_DEF( PORT2,  7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* PMON - INA3221  */
#define GPIO_PMON_CRITICAL              PIN_DEF( PORT4,  17, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PMON_PV                    PIN_DEF( PORT4,  18, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PMON_TC                    PIN_DEF( PORT4,  19, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PMON_WARNING               PIN_DEF( PORT4,  24, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FMC PG */
#define GPIO_FMC1_PG_M2C                PIN_DEF( PORT3,  8, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_PG_M2C                PIN_DEF( PORT3,  9, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC1_PG_C2M                PIN_DEF( PORT3, 10, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FMC2_PG_C2M                PIN_DEF( PORT3, 11, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* RTM */
/* NOTE: Due to a hardware limitation, pins 29 and 30 from port 0 will have the same direction, even if set differently in the register */
#define GPIO_RTM_PS                     PIN_DEF( PORT0, 29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_RTM_I2C_EN                 PIN_DEF( PORT4,  2, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* Ethernet */
#define GPIO_PHY_RESETn                 PIN_DEF( PORT0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_PHY_MII1_MODE              PIN_DEF( PORT0, 31, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_PHY_MII1_INT               PIN_DEF( PORT1, 18, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FPGA */
#define GPIO_FPGA_PROGRAM_B             PIN_DEF( PORT0,  4, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_DONE_B                PIN_DEF( PORT0,  5, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_RESETn                PIN_DEF( PORT4,  9, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_INITB                 PIN_DEF( PORT0, 13, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_STATUS                PIN_DEF( PORT0, 14, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_EN_FLASH_UPDATE            PIN_DEF( PORT0, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

#define GPIO_PS_ERR_OUT                 PIN_DEF( PORT4, 27, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PS_ERR_STAT                PIN_DEF( PORT4, 28, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* MMC ENABLE# */
#define GPIO_MMC_ENABLE                 PIN_DEF( PORT4,  8, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* OVERTEMP */
#define GPIO_OVERTEMPn                  PIN_DEF( PORT4, 11, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* JTAG */
#define GPIO_SOC_JTAG_OVERRIDE          PIN_DEF( PORT4,  4, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FMC1_JTAG_OVERRIDE         PIN_DEF( PORT4,  5, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FMC2_JTAG_OVERRIDE         PIN_DEF( PORT4,  6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_RTM_JTAG_OVERRIDE          PIN_DEF( PORT4,  7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* EEPROM */
#define GPIO_EEPROM_WP                  PIN_DEF( PORT4, 14, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* FMC PRSNT */
#define GPIO_FMC1_PRSNT_M2C             PIN_DEF( PORT3, 6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_PRSNT_M2C             PIN_DEF( PORT3, 7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FMC CLK */
#define GPIO_FMC1_CLK_DIR               PIN_DEF( PORT3, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_CLK_DIR               PIN_DEF( PORT3, 13, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* GPIO EXPANDER - CLK */
#define GPIO_CLK_MG_INT                 PIN_DEF( PORT3, 15, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Hardware ID */
#define GPIO_HW_ID0                     PIN_DEF( PORT3, 23, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID1                     PIN_DEF( PORT3, 24, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID2                     PIN_DEF( PORT3, 25, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_HW_ID3                     PIN_DEF( PORT3, 26, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )


/* Pin initialization (config) list */
#define PIN_CFG_LIST                            \
        I2C0_SDA,                               \
        I2C0_SCL,                               \
        I2C1_SDA,                               \
        I2C1_SCL,                               \
        I2C2_SDA,                               \
        I2C2_SCL,                               \
        MMC_UART1_TXD,                          \
        MMC_UART1_RXD,                          \
        MMC_UART4_TXD,                          \
        MMC_UART4_RXD,                          \
        UART_DEBUG_TXD,                         \
        UART_DEBUG_RXD,                         \
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
        FLASH_UPD_MOSI,                         \
        ENET_PHY_MII1_TXD0,                     \
        ENET_PHY_MII1_TXD1,                     \
        ENET_PHY_MII1_TXD2,                     \
        ENET_PHY_MII1_TXD3,                     \
        ENET_PHY_MII1_TX_EN,                    \
        ENET_PHY_MII1_TX_CLK,                   \
        ENET_PHY_MII1_COL,                      \
        ENET_PHY_MII1_CRS,                      \
        ENET_PHY_MII1_RXD0,                     \
        ENET_PHY_MII1_RXD1,                     \
        ENET_PHY_MII1_RXD2,                     \
        ENET_PHY_MII1_RXD3,                     \
        ENET_PHY_MII1_RX_DV,                    \
        ENET_PHY_MII1_RX_ER,                    \
        ENET_PHY_MII1_RX_CLK,                   \
        ENET_PHY_MII1_MDC,                      \
        ENET_PHY_MII1_MDIO,                     \
        TRACEDATA3,                             \
        TRACEDATA2,                             \
        TRACEDATA1,                             \
        TRACEDATA0,                             \
        TRACECLK,                               \
        GPIO_I2C_MUX_ADDR1,                     \
        GPIO_I2C_MUX_ADDR2,                     \
        GPIO_SW_RESETn,                         \
        GPIO_BOOT_MODE0,                        \
        GPIO_BOOT_MODE1,                        \
        GPIO_BOOT_MODE2,                        \
        GPIO_BOOT_MODE3,                        \
        GPIO_LPC_ISP_SWn,                       \
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
        GPIO_PGOOD_P5V0,                        \
        GPIO_SDRAM_PGOOD,                       \
        GPIO_PMON_CRITICAL,                     \
        GPIO_PMON_PV,                           \
        GPIO_PMON_TC,                           \
        GPIO_PMON_WARNING,                      \
        GPIO_FMC1_PG_M2C,                       \
        GPIO_FMC1_PG_C2M,                       \
        GPIO_FMC2_PG_M2C,                       \
        GPIO_FMC2_PG_C2M,                       \
        GPIO_RTM_PS,                            \
        GPIO_RTM_I2C_EN,                        \
        GPIO_PHY_RESETn,                        \
        GPIO_PHY_MII1_MODE,                     \
        GPIO_PHY_MII1_INT,                      \
        GPIO_FPGA_PROGRAM_B,                    \
        GPIO_FPGA_DONE_B,                       \
        GPIO_FPGA_RESETn,                       \
        GPIO_FPGA_INITB,                        \
        GPIO_FPGA_STATUS,                       \
        GPIO_EN_FLASH_UPDATE,                   \
        GPIO_PS_ERR_OUT,                        \
        GPIO_PS_ERR_STAT,                       \
        GPIO_MMC_ENABLE,                        \
        GPIO_OVERTEMPn,                         \
        GPIO_SOC_JTAG_OVERRIDE,                 \
        GPIO_FMC1_JTAG_OVERRIDE,                \
        GPIO_FMC2_JTAG_OVERRIDE,                \
        GPIO_RTM_JTAG_OVERRIDE,                 \
        GPIO_EEPROM_WP,                         \
        GPIO_FMC1_PRSNT_M2C,                    \
        GPIO_FMC2_PRSNT_M2C,                    \
        GPIO_FMC1_CLK_DIR,                      \
        GPIO_FMC2_CLK_DIR,                      \
        GPIO_CLK_MG_INT,                        \
        GPIO_HW_ID0,                            \
        GPIO_HW_ID1,                            \
        GPIO_HW_ID2,                            \
        GPIO_HW_ID3                             \

#endif
/**
 * @}
 */
