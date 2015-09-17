/*
 * board_defs.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
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
 */

#ifndef BOARD_DEFS_H_
#define BOARD_DEFS_H_

#define AFC_V3

#ifdef AFC_V3

/* I2C Pins definitions */

#define I2C0_PORT       0
#define I2C0_SDA_PIN    27
#define I2C0_SCL_PIN    28
#define I2C0_PIN_FUNC   1

#define I2C1_PORT       0
#define I2C1_SDA_PIN    0
#define I2C1_SCL_PIN    1
#define I2C1_PIN_FUNC   3

#define I2C2_PORT       0
#define I2C2_SDA_PIN    10
#define I2C2_SCL_PIN    11
#define I2C2_PIN_FUNC   2

/* Geographic Address pin definitions */
#define GA0_PORT        1
#define GA1_PORT        1
#define GA2_PORT        1
#define GA_TEST_PORT    1
#define GA0_PIN         0
#define GA1_PIN         1
#define GA2_PIN         4
#define GA_TEST_PIN     8

/* Led pin definitions */
#define LEDBLUE_PORT    1
#define LEDBLUE_PIN     9
#define LEDGREEN_PORT   1
#define LEDGREEN_PIN    10
#define LEDRED_PORT     1
#define LEDRED_PIN      25
#define LED_PIN_FUNC    0

/* Hot swap handle */
#define HOT_SWAP_HANDLE_PORT 2
#define HOT_SWAP_HANDLE_PIN  13
#define HOT_SWAP_HANDLE_FUNC 0

/* DCDC converters */
#define GPIO_EN_P1V2_PORT       0
#define GPIO_EN_P1V2_PIN	23
#define GPIO_EN_P1V8_PORT       0
#define GPIO_EN_P1V8_PIN	24

#define GPIO_EM_FMC1_P12V_PORT  0
#define GPIO_EM_FMC1_P12V_PIN   4
#define GPIO_EN_FMC1_P3V3_PORT  0
#define GPIO_EN_FMC1_P3V3_PIN	25
#define GPIO_EN_FMC1_PVADJ_PORT 1
#define GPIO_EN_FMC1_PVADJ_PIN  31

#define GPIO_EM_FMC2_P12V_PORT  0
#define GPIO_EM_FMC2_P12V_PIN   5
#define GPIO_EN_FMC2_P3V3_PORT  0
#define GPIO_EN_FMC2_P3V3_PIN	26
#define GPIO_EN_FMC2_PVADJ_PORT 1
#define GPIO_EN_FMC2_PVADJ_PIN	28

#define GPIO_EN_P3V3_PORT       1
#define GPIO_EN_P3V3_PIN	27
#define GPIO_EN_1V5_VTT_PORT    1
#define GPIO_EN_1V5_VTT_PIN	29
#define GPIO_EN_P1V0_PORT       3
#define GPIO_EN_P1V0_PIN	25

#define GPIO_PROGRAM_B_PIN	17
#define GPIO_PROGRAM_B_PORT     0
#define GPIO_DONE_B_PIN		22
#define GPIO_DONE_B_PORT        0
#define GPIO_PGOOD_P1V0_PIN	26
#define GPIO_PGOOD_P1V0_PORT    3

#endif

#ifdef MBED

/* I2C Pins definitions */

#define I2C0_PORT       0
#define I2C0_SDA_PIN    27
#define I2C0_SCL_PIN    28
#define I2C0_PIN_FUNC   1

#define I2C1_PORT       0
#define I2C1_SDA_PIN    0
#define I2C1_SCL_PIN    1
#define I2C1_PIN_FUNC   3

#define I2C2_PORT       0
#define I2C2_SDA_PIN    10
#define I2C2_SCL_PIN    11
#define I2C2_PIN_FUNC   2

/* Geographic Address pin definitions */
#define GA_PORT         1
#define GA0_PIN         0
#define GA1_PIN         1
#define GA2_PIN         4
#define GA_TEST_PIN     8

#define LEDBLUE_PORT    1
#define LEDBLUE_PIN     18
#define LEDGREEN_PORT   1
#define LEDGREEN_PIN    20
#define LEDRED_PORT     1
#define LEDRED_PIN      21

#endif

#endif
