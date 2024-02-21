/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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
 * @defgroup AFC_V4_0 AFCv4.0 Board Port
 * @ingroup BOARD_PORTS
 */

/**
 * @file afc-v4/pin_mapping.h
 * @brief Hardware pin definitions for AFCv4.0
 *
 * @ingroup AFC_V4_0_PIN_MAPPING
 */

/**
 * @defgroup AFC_V4_0_PIN_MAPPING AFCv4.0 Pin Mapping
 * @ingroup AFC_V4_0
 * @{
 */

#ifndef PIN_MAPPING_H_
#define PIN_MAPPING_H_

/* SPI Interfaces */
#define FPGA_SPI        0

/* UART Interfaces */
#define UART_DEBUG      0

/* Pin definitions */

/* I2C ports */
#define I2C0_SDA                        PIN_DEF( PORT0, 27, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C0_SCL                        PIN_DEF( PORT0, 28, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define I2C1_SDA                        PIN_DEF( PORT0,  0, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define I2C1_SCL                        PIN_DEF( PORT0,  1, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define I2C2_SDA                        PIN_DEF( PORT0, 10, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )
#define I2C2_SCL                        PIN_DEF( PORT0, 11, (IOCON_FUNC2 | IOCON_MODE_INACT), NON_GPIO )

/* UART Debug port */
#define UART_DEBUG_TXD                  PIN_DEF( PORT0, 2, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )
#define UART_DEBUG_RXD                  PIN_DEF( PORT0, 3, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )

/* FPGA SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SSP0_SCK                        PIN_DEF( PORT1, 20, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_SSEL                       PIN_DEF( PORT1, 21, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SSP0_MISO                       PIN_DEF( PORT1, 23, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SSP0_MOSI                       PIN_DEF( PORT1, 24, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )

/* SPI Legacy port - should be updated to SSP interface */
/* DAC SPI Port (SSEL is GPIO for word transfers larger than 8bits) */
#define SPI_SCK                         PIN_DEF( PORT0, 15, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define SPI_SSEL                        PIN_DEF( PORT0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define SPI_MOSI                        PIN_DEF( PORT0, 18, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )

/* Tracedata */
#define TRACEDATA3                      PIN_DEF( PORT2,  2, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA2                      PIN_DEF( PORT2,  3, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA1                      PIN_DEF( PORT2,  4, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define TRACEDATA0                      PIN_DEF( PORT2,  5, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )
#define TRACECLK                        PIN_DEF( PORT2,  6, (IOCON_FUNC3 | IOCON_MODE_INACT), NON_GPIO )

/*ADC Payload detector*/
#define ADC_PAYLOAD_DETECTOR            PIN_DEF( PORT0,  24, (IOCON_FUNC1 | IOCON_MODE_INACT), NON_GPIO )


/* GPIO definitions */

/* I2C MUX control pins */
#define GPIO_I2C_MUX_ADDR1              PIN_DEF( PORT0,  4, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_I2C_MUX_ADDR2              PIN_DEF( PORT0,  5, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN), GPIO_DIR_OUTPUT )
#define GPIO_I2C_SW_RESETn              PIN_DEF( PORT0, 19, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* LPC_ISPn RTS */
#define GPIO_RTS                        PIN_DEF( PORT2,  10, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Power Good pins */
#define GPIO_PG_RESETn                  PIN_DEF( PORT1,  22, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_PGOOD_P1V0                 PIN_DEF( PORT3,  26, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

#define GPIO_AMC_RTM_CRITICAL           PIN_DEF( PORT1,  26, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_AMC_RTM_TC                 PIN_DEF( PORT1,  28, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_AMC_RTM_WARNING            PIN_DEF( PORT1,  29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_AMC_RTM_PV                 PIN_DEF( PORT1,  27, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FMC Power Good pins */
#define GPIO_FMC1_PG_M2C                PIN_DEF( PORT1, 16, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_PG_M2C                PIN_DEF( PORT1, 17, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC1_PG_C2M                PIN_DEF( PORT1, 18, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FMC2_PG_C2M                PIN_DEF( PORT1, 19, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

#define GPIO_FMC1_CRITICAL              PIN_DEF( PORT0,  23, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC1_TC                    PIN_DEF( PORT0,  25, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC1_WARNING               PIN_DEF( PORT0,  26, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC1_PV                    PIN_DEF( PORT1,  31, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

#define GPIO_FMC2_CRITICAL              PIN_DEF( PORT2,  7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_TC                    PIN_DEF( PORT4,  28, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_WARNING               PIN_DEF( PORT4,  29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_PV                    PIN_DEF( PORT3,  25, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

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
#define GPIO_FRONT_BUTTON               PIN_DEF( PORT2, 12, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Hot swap handle */
#define GPIO_HOT_SWAP_HANDLE            PIN_DEF( PORT2, 13, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FPGA Control */
#define GPIO_FPGA_DONE_B                PIN_DEF( PORT0, 22, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FPGA_INITB                 PIN_DEF( PORT0, 20, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FPGA_RESET                 PIN_DEF( PORT2,  9, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* VADJ DAC */
#define GPIO_DAC_VADJ_RST               PIN_DEF( PORT0, 21, (IOCON_FUNC0 | IOCON_MODE_PULLUP), GPIO_DIR_OUTPUT )

/* MMC ENABLE# */
#define GPIO_MMC_ENABLE                 PIN_DEF( PORT2,  8, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* Overtemp signal */
#define GPIO_OVERTEMPn                  PIN_DEF( PORT2, 11, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* JTAG */
#define GPIO_FMC1_JTAG_Override         PIN_DEF( PORT2,  1, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_FMC2_JTAG_Override         PIN_DEF( PORT0,  8, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )
#define GPIO_RTM_JTAG_Override          PIN_DEF( PORT2,  0, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* EEPROM WP signal */
#define GPIO_EEPROM_WP                  PIN_DEF( PORT1,  30, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_OUTPUT )

/* FMC CLK signals */
#define GPIO_CLK_DIR_FMC2               PIN_DEF( PORT0,  6, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_CLK_DIR_FMC1               PIN_DEF( PORT0,  7, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* FMC Present signals */
#define GPIO_FMC1_PRSNT_M2C             PIN_DEF( PORT1, 14, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )
#define GPIO_FMC2_PRSNT_M2C             PIN_DEF( PORT1, 15, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )

/* RTM */
#define GPIO_RTM_PS                     PIN_DEF( PORT0, 29, (IOCON_FUNC0 | IOCON_MODE_INACT), GPIO_DIR_INPUT )


/* Pin initialization (config) list */
#define PIN_CFG_LIST                            \
         I2C0_SDA,                              \
         I2C0_SCL,                              \
         I2C1_SDA,                              \
         I2C1_SCL,                              \
         I2C2_SDA,                              \
         I2C2_SCL,                              \
         UART_DEBUG_TXD,                        \
         UART_DEBUG_RXD,                        \
         SSP0_SCK,                              \
         SSP0_SSEL,                             \
         SSP0_MISO,                             \
         SSP0_MOSI,                             \
         SPI_SCK,                               \
         SPI_SSEL,                              \
         SPI_MOSI,                              \
         TRACEDATA3,                            \
         TRACEDATA2,                            \
         TRACEDATA1,                            \
         TRACEDATA0,                            \
         TRACECLK,                              \
         ADC_PAYLOAD_DETECTOR,                  \
         GPIO_I2C_MUX_ADDR1,                    \
         GPIO_I2C_MUX_ADDR2,                    \
         GPIO_I2C_SW_RESETn,                    \
         GPIO_RTS,                              \
         GPIO_PG_RESETn,                        \
         GPIO_PGOOD_P1V0,                       \
         GPIO_AMC_RTM_CRITICAL,                 \
         GPIO_AMC_RTM_TC,                       \
         GPIO_AMC_RTM_WARNING,                  \
         GPIO_AMC_RTM_PV,                       \
         GPIO_FMC1_PG_M2C,                      \
         GPIO_FMC2_PG_M2C,                      \
         GPIO_FMC1_PG_C2M,                      \
         GPIO_FMC2_PG_C2M,                      \
         GPIO_FMC1_CRITICAL,                    \
         GPIO_FMC1_TC,                          \
         GPIO_FMC1_WARNING,                     \
         GPIO_FMC1_PV,                          \
         GPIO_FMC2_CRITICAL,                    \
         GPIO_FMC2_TC,                          \
         GPIO_FMC2_WARNING,                     \
         GPIO_FMC2_PV,                          \
         GPIO_GA0,                              \
         GPIO_GA1,                              \
         GPIO_GA2,                              \
         GPIO_GA_TEST,                          \
         GPIO_LEDBLUE,                          \
         GPIO_LEDGREEN,                         \
         GPIO_LEDRED,                           \
         GPIO_FRONT_BUTTON,                     \
         GPIO_HOT_SWAP_HANDLE,                  \
         GPIO_FPGA_DONE_B,                      \
         GPIO_FPGA_INITB,                       \
         GPIO_FPGA_RESET,                       \
         GPIO_DAC_VADJ_RST,                     \
         GPIO_MMC_ENABLE,                       \
         GPIO_OVERTEMPn,                        \
         GPIO_FMC1_JTAG_Override,               \
         GPIO_FMC2_JTAG_Override,               \
         GPIO_RTM_JTAG_Override,                \
         GPIO_EEPROM_WP,                        \
         GPIO_CLK_DIR_FMC2,                     \
         GPIO_CLK_DIR_FMC1,                     \
         GPIO_FMC1_PRSNT_M2C,                   \
         GPIO_FMC2_PRSNT_M2C,                   \
         GPIO_RTM_PS

/**
 * @}
 */
#include <stdint.h>

typedef struct external_gpio {
    uint8_t port_num;
    uint8_t pin_num;
} external_gpio_t;

extern const external_gpio_t ext_gpios[16];

enum {
	EXT_GPIO_EN_P1V0,
	EXT_GPIO_EN_P1V8,
	EXT_GPIO_EN_P3V3,
	EXT_GPIO_EN_FMC1_PVADJ,
	EXT_GPIO_EN_FMC2_PVADJ,
	EXT_GPIO_P1V5_VTT_EN,
	EXT_GPIO_EN_P1V2,
	EXT_GPIO_EN_FMC1_P12V,
	EXT_GPIO_EN_FMC1_P3V3,
	EXT_GPIO_EN_FMC2_P12V,
	EXT_GPIO_EN_FMC2_P3V3,
	EXT_GPIO_EN_RTM_PWR,
	EXT_GPIO_EN_RTM_MP,
	EXT_GPIO_FPGA_I2C_RESET,
	EXT_GPIO_DAC_VADJ_RSTn,
	EXT_GPIO_PROGRAM_B,
};

#endif
