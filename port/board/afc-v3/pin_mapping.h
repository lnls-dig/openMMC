/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2016  Henrique Silva <henrique.silva@lnls.br>
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
 * @defgroup AFC_TIMING AFC Timing Board Port
 * @ingroup BOARD_PORTS
 */

/**
 * @file afc-timing/pin_mapping.h
 * @brief Hardware pin definitions for AFC Timing
 *
 * @ingroup AFC_TIMING_PIN_MAPPING
 */

/**
 * @defgroup AFC_TIMING_PIN_MAPPING AFC Timing Pin Mapping
 * @ingroup AFC_TIMING
 * @{
 */

#ifndef PIN_MAPPING_H_
#define PIN_MAPPING_H_

/* SPI Interfaces */
#define FPGA_SPI        0
#define FLASH_SPI       1

/* UART Interfaces */
#define UART_DEBUG      3

/* Pin definitions */

/* I2C ports */
#define I2C0_SDA                        PIN_DEF( PORT0, 27, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C0_SCL                        PIN_DEF( PORT0, 28, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C1_SDA                        PIN_DEF( PORT0,  0, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define I2C1_SCL                        PIN_DEF( PORT0,  1, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define I2C2_SDA                        PIN_DEF( PORT0, 10, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define I2C2_SCL                        PIN_DEF( PORT0, 11, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

/* UART Debug port */
#define UART_DEBUG_TXD                  PIN_DEF( PORT4, 28, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_RXD                  PIN_DEF( PORT4, 29, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )

/* SSP Ports */
/* FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP0_SCK                        PIN_DEF( PORT1, 20, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_SSEL                       PIN_DEF( PORT1, 21, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP0_MISO                       PIN_DEF( PORT1, 23, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_MOSI                       PIN_DEF( PORT1, 24, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
/* Flash SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP1_SCK                        PIN_DEF( PORT0,  7, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_SSEL                       PIN_DEF( PORT0,  6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP1_MISO                       PIN_DEF( PORT0,  8, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define SSP1_MOSI                       PIN_DEF( PORT0,  9, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

/* SPI Legacy port - should be updated to SSP interface */
/* DAC SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SPI_SCK                         PIN_DEF( PORT0, 15, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SPI_SSEL                        PIN_DEF( PORT0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
//#define SPI_MISO                        PIN_DEF( PORT0, 17, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SPI_MOSI                        PIN_DEF( PORT0, 18, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )

/* GPIO definitions */
/* Geographic Address pin definitions */
#define GPIO_GA0                        PIN_DEF( PORT1,  0, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA1                        PIN_DEF( PORT1,  1, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA2                        PIN_DEF( PORT1,  4, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_GA_TEST                    PIN_DEF( PORT1,  8, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* Board LEDs */
#define GPIO_LEDBLUE                    PIN_DEF( PORT1,  9, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_LEDGREEN                   PIN_DEF( PORT1, 10, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_LEDRED                     PIN_DEF( PORT1, 25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* Front Panel BUTTON */
#define GPIO_FRONT_BUTTON               PIN_DEF( PORT2, 12, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* Hot swap handle */
#define GPIO_HOT_SWAP_HANDLE            PIN_DEF( PORT2, 13, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* FPGA DCDC converters */
#define GPIO_EN_P1V0                    PIN_DEF( PORT3, 25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P1V2                    PIN_DEF( PORT0, 23, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P1V8                    PIN_DEF( PORT0, 24, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P3V3                    PIN_DEF( PORT1, 27, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_P1V5_VTT                PIN_DEF( PORT1, 29, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_DCDC_PGOOD                 PIN_DEF( PORT3, 26, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_INPUT )

/* FMC1 DCDC Converters */
#define GPIO_EN_FMC1_P12V               PIN_DEF( PORT0,  4, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_FMC1_P3V3               PIN_DEF( PORT0, 25, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_FMC1_PVADJ              PIN_DEF( PORT1, 31, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* FMC2 DCDC Converters */
#define GPIO_EN_FMC2_P12V               PIN_DEF( PORT0,  5, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_FMC2_P3V3               PIN_DEF( PORT0, 26, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_EN_FMC2_PVADJ              PIN_DEF( PORT1, 28, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* RTM */
#define GPIO_EN_RTM_PWR                 PIN_DEF( PORT1, 30, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
/* NOTE: Due to a hardware limitation, pins 29 and 30 from port 0 will have the same direction, even if set differently in the register */
#define GPIO_RTM_PS                     PIN_DEF( PORT0, 29, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )
#define GPIO_EN_RTM_I2C                 PIN_DEF( PORT0, 30, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )

/* FPGA Control */
#define GPIO_FPGA_PROGRAM_B             PIN_DEF( PORT0, 17, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_DONE_B                PIN_DEF( PORT0, 22, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_RESET                 PIN_DEF( PORT2,  9, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_INITB                 PIN_DEF( PORT0, 20, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FLASH_CS_MUX               PIN_DEF( PORT0, 19, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* VADJ DAC */
#define GPIO_DAC_VADJ_RST               PIN_DEF( PORT0, 21, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_DAC_VADJ_CSN               PIN_DEF( PORT0, 16, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )

/* ADN4604 Clock Switch */
#define GPIO_ADN_UPDATE                 PIN_DEF( PORT1, 26, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_ADN_RESETN                 PIN_DEF( PORT1, 22, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* SCANSTA1101 JTAG Switch */
#define GPIO_SCANSTA1101_ADDR0          PIN_DEF( PORT2,  0, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR1          PIN_DEF( PORT2,  1, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR2          PIN_DEF( PORT2,  2, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR3          PIN_DEF( PORT2,  3, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR4          PIN_DEF( PORT2,  4, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR5          PIN_DEF( PORT2,  5, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_ADDR6          PIN_DEF( PORT2,  6, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_SCANSTA1101_TRIST_B        PIN_DEF( PORT2,  7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* MMC ENABLE# */
#define GPIO_MMC_ENABLE                 PIN_DEF( PORT2,  8, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* Overtemp signal */
#define GPIO_OVERTEMPn                  PIN_DEF( PORT2, 10, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* FMC Present signals */
#define GPIO_FMC1_PRSNT_M2C             PIN_DEF( PORT1, 14, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )
#define GPIO_FMC2_PRSNT_M2C             PIN_DEF( PORT1, 15, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )

/* FMC Power Good signals */
#define GPIO_FMC1_PG_M2C                PIN_DEF( PORT1, 16, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )
#define GPIO_FMC2_PG_M2C                PIN_DEF( PORT1, 17, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_INPUT )
#define GPIO_FMC1_PG_C2M                PIN_DEF( PORT1, 18, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )
#define GPIO_FMC2_PG_C2M                PIN_DEF( PORT1, 19, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )

/* Pin initialization (config) list */
#define PIN_CFG_LIST                            \
    I2C0_SDA,                                   \
        I2C0_SCL,                               \
        I2C1_SDA,                               \
        I2C1_SCL,                               \
        I2C2_SDA,                               \
        I2C2_SCL,                               \
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
        SPI_SCK,                                \
        SPI_SSEL,                               \
        SPI_MOSI,                               \
        GPIO_GA0,                               \
        GPIO_GA1,                               \
        GPIO_GA2,                               \
        GPIO_GA_TEST,                           \
        GPIO_LEDBLUE,                           \
        GPIO_LEDGREEN,                          \
        GPIO_LEDRED,                            \
        GPIO_FRONT_BUTTON,                      \
        GPIO_HOT_SWAP_HANDLE,                   \
        GPIO_EN_P1V0,                           \
        GPIO_EN_P1V2,                           \
        GPIO_EN_P1V8,                           \
        GPIO_EN_P3V3,                           \
        GPIO_EN_P1V5_VTT,                       \
        GPIO_DCDC_PGOOD,                        \
        GPIO_EN_FMC1_P12V,                      \
        GPIO_EN_FMC1_P3V3,                      \
        GPIO_EN_FMC1_PVADJ,                     \
        GPIO_EN_FMC2_P12V,                      \
        GPIO_EN_FMC2_P3V3,                      \
        GPIO_EN_FMC2_PVADJ,                     \
        GPIO_EN_RTM_PWR,                        \
        GPIO_RTM_PS,                            \
        GPIO_EN_RTM_I2C,                        \
        GPIO_FPGA_PROGRAM_B,                    \
        GPIO_FPGA_DONE_B,                       \
        GPIO_FPGA_RESET,                        \
        GPIO_FPGA_INITB,                        \
        GPIO_FLASH_CS_MUX,                      \
        GPIO_DAC_VADJ_RST,                      \
        GPIO_DAC_VADJ_CSN,                      \
        GPIO_ADN_UPDATE,                        \
        GPIO_ADN_RESETN,                        \
        GPIO_SCANSTA1101_ADDR0,                 \
        GPIO_SCANSTA1101_ADDR1,                 \
        GPIO_SCANSTA1101_ADDR2,                 \
        GPIO_SCANSTA1101_ADDR3,                 \
        GPIO_SCANSTA1101_ADDR4,                 \
        GPIO_SCANSTA1101_ADDR5,                 \
        GPIO_SCANSTA1101_ADDR6,                 \
        GPIO_SCANSTA1101_TRIST_B,               \
        GPIO_MMC_ENABLE,                        \
        GPIO_FMC1_PRSNT_M2C,                    \
        GPIO_FMC2_PRSNT_M2C,                    \
        GPIO_FMC1_PG_M2C,                       \
        GPIO_FMC2_PG_M2C,                       \
        GPIO_FMC1_PG_C2M,                       \
        GPIO_FMC2_PG_C2M

#endif

/**
 * @}
 */
