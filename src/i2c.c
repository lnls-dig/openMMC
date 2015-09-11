/*
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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

/*!
 * @file i2c.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date August 2015
 *
 * @brief Implementation of a generic I2C driver using FreeRTOS features
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* C Standard includes */
#include "stdio.h"
#include "string.h"

/* Project includes */
#include "i2c.h"
#include "ipmb.h"
#include "ipmi.h"
#include "chip.h"
#include "board_defs.h"
#include "port.h"

/* Project definitions */

/*! @brief Configuration struct for each I2C interface */
xI2C_Config i2c_cfg[] = {
    {
        .reg = LPC_I2C0,
        .irq = I2C0_IRQn,
        .mode = I2C_Mode_IPMB,
        .pins = {
            .sda_port = I2C0_PORT,
            .sda_pin = I2C0_SDA_PIN,
            .scl_port = I2C0_PORT,
            .scl_pin = I2C0_SCL_PIN,
            .pin_func = I2C0_PIN_FUNC
        },
    },
    {
        .reg = LPC_I2C1,
        .irq = I2C1_IRQn,
        .mode = I2C_Mode_Local_Master,
        .pins = {
            .sda_port = I2C1_PORT,
            .sda_pin = I2C1_SDA_PIN,
            .scl_port = I2C1_PORT,
            .scl_pin = I2C1_SCL_PIN,
            .pin_func = I2C1_PIN_FUNC
        },
    },
    {
        .reg = LPC_I2C2,
        .irq = I2C2_IRQn,
        .mode = I2C_Mode_Local_Master,
        .pins = {
            .sda_port = I2C2_PORT,
            .sda_pin = I2C2_SDA_PIN,
            .scl_port = I2C2_PORT,
            .scl_pin = I2C2_SCL_PIN,
            .pin_func = I2C2_PIN_FUNC
        },
    }
};

/*! @brief Array of mutexes to access #i2c_cfg global struct
 *
 * Each I2C interface has its own mutex and it must be taken
 * before setting/reading any field from #i2c_cfg struct,
 * since it's used by multiple tasks simultaneously
 */
static SemaphoreHandle_t I2C_mutex[3];


void vI2CInit( I2C_ID_T i2c_id, I2C_Mode mode )
{
    char pcI2C_Tag[4];
    uint8_t sla_addr;

    sprintf( pcI2C_Tag, "I2C%u", i2c_id );
    /*! @todo Maybe wrap these functions, or use some board-specific defines
     * so this code is generic enough to be applied on other hardware.
     * Example: (if using LPC17xx and LPCOpen library)
     * @code
     * #define PIN_FUNC_CFG( port, pin, func ) Chip_IOCON_PinMux(...)
     * @endcode
    */
    Chip_IOCON_PinMux( LPC_IOCON, i2c_cfg[i2c_id].pins.sda_port, i2c_cfg[i2c_id].pins.sda_pin, IOCON_MODE_INACT, i2c_cfg[i2c_id].pins.pin_func );
    Chip_IOCON_PinMux( LPC_IOCON, i2c_cfg[i2c_id].pins.scl_port, i2c_cfg[i2c_id].pins.scl_pin, IOCON_MODE_INACT, i2c_cfg[i2c_id].pins.pin_func );
    Chip_IOCON_EnableOD( LPC_IOCON, i2c_cfg[i2c_id].pins.sda_port, i2c_cfg[i2c_id].pins.sda_pin );
    Chip_IOCON_EnableOD( LPC_IOCON, i2c_cfg[i2c_id].pins.scl_port, i2c_cfg[i2c_id].pins.scl_pin );
    NVIC_SetPriority(i2c_cfg[i2c_id].irq, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ( i2c_cfg[i2c_id].irq );

    /* Create mutex for accessing the shared memory (i2c_cfg) */
    I2C_mutex[i2c_id] = xSemaphoreCreateMutex();

    /* Make sure that the mutex is freed */
    xSemaphoreGive( I2C_mutex[i2c_id] );

    /* Set I2C operating mode */
    if( xSemaphoreTake( I2C_mutex[i2c_id], 0 ) ) {
        i2c_cfg[i2c_id].mode = mode;
        xSemaphoreGive( I2C_mutex[i2c_id] );
    }

    /* Enable and configure I2C clock */
    Chip_I2C_Init( i2c_id );
    Chip_I2C_SetClockRate( i2c_id, 100000 );

    if ( mode == I2C_Mode_IPMB ) {
        /* Configure Slave Address */
        sla_addr = get_ipmb_addr( );

        port_I2C_Slave_Setup( i2c_id, ipmb_addr, i2c_cfg[i2c_id].msg.rx_data, i2cMAX_MSG_LENGTH );

    }

} /* End of vI2C_Init */

/*
 *==============================================================
 * MMC ADDRESSING
 *==============================================================
*/

/*! @brief Table holding all possible address values in IPMB specification
 * @see get_ipmb_addr()
 */
unsigned char IPMBL_TABLE[IPMBL_TABLE_SIZE] = {
    0x70, 0x8A, 0x72, 0x8E, 0x92, 0x90, 0x74, 0x8C, 0x76,
    0x98, 0x9C, 0x9A, 0xA0, 0xA4, 0x88, 0x9E, 0x86, 0x84,
    0x78, 0x94, 0x7A, 0x96, 0x82, 0x80, 0x7C, 0x7E, 0xA2 };

/*! The state of each GA signal is represented by G (grounded), U (unconnected), 
 *  or P (pulled up to Management Power).
 *
 *  The MMC drives P1 low and reads the GA lines. The MMC then drives P1 high and
 *  reads the GA lines. Any line that changes state between the two reads indicate
 *  an unconnected (U) pin.
 *
 *  The IPMB-L address of a Module can be calculated as (70h + Site Number x 2). <br>
 *  G = 0, P = 1, U = 2 <br>
 *  | Pin | Ternary | Decimal | Address |
 *  |:---:|:-------:|:-------:|:-------:|
 *  | GGG | 000 | 0  | 0x70 |
 *  | GGP | 001 | 1  | 0x8A |
 *  | GGU | 002 | 2  | 0x72 |
 *  | GPG | 010 | 3  | 0x8E |
 *  | GPP | 011 | 4  | 0x92 |
 *  | GPU | 012 | 5  | 0x90 |
 *  | GUG | 020 | 6  | 0x74 |
 *  | GUP | 021 | 7  | 0x8C |
 *  | GUU | 022 | 8  | 0x76 |
 *  | PGG | 100 | 9  | 0x98 |
 *  | PGP | 101 | 10 | 0x9C |
 *  | PGU | 102 | 11 | 0x9A |
 *  | PPG | 110 | 12 | 0xA0 |
 *  | PPP | 111 | 13 | 0xA4 |
 *  | PPU | 112 | 14 | 0x88 |
 *  | PUG | 120 | 15 | 0x9E |
 *  | PUP | 121 | 16 | 0x86 |
 *  | PUU | 122 | 17 | 0x84 |
 *  | UGG | 200 | 18 | 0x78 |
 *  | UGP | 201 | 19 | 0x94 |
 *  | UGU | 202 | 20 | 0x7A |
 *  | UPG | 210 | 21 | 0x96 |
 *  | UPP | 211 | 22 | 0x82 |
 *  | UPU | 212 | 23 | 0x80 |
 *  | UUG | 220 | 24 | 0x7C |
 *  | UUP | 221 | 25 | 0x7E |
 *  | UUU | 222 | 26 | 0xA2 |
 */
#define GPIO_GA_DELAY 10
uint8_t get_ipmb_addr( void )
{
    uint8_t ga0, ga1, ga2;
    uint8_t index;

    /* Set the test pin and read all GA pins */
    Chip_GPIO_SetPinState(LPC_GPIO, GA_TEST_PORT, GA_TEST_PIN, 1);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GA_TEST_PIN changes */
    {
		uint8_t i;
		for (i = 0; i < GPIO_GA_DELAY; i++)
			asm volatile ("nop");
	}


    ga0 = Chip_GPIO_GetPinState(LPC_GPIO, GA0_PORT, GA0_PIN);
    ga1 = Chip_GPIO_GetPinState(LPC_GPIO, GA1_PORT, GA1_PIN);
    ga2 = Chip_GPIO_GetPinState(LPC_GPIO, GA2_PORT, GA2_PIN);

    /* Clear the test pin and see if any GA pin has changed is value,
     * meaning that it is unconnected */
    Chip_GPIO_SetPinState(LPC_GPIO, GA_TEST_PORT, GA_TEST_PIN, 0);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GA_TEST_PIN changes */
    {
		uint8_t i;
		for (i = 0; i < GPIO_GA_DELAY; i++)
			asm volatile ("nop");
	}


    if ( ga0 != Chip_GPIO_GetPinState(LPC_GPIO, GA0_PORT, GA0_PIN) )
    {
        ga0 = UNCONNECTED;
    }

    if ( ga1 != Chip_GPIO_GetPinState(LPC_GPIO, GA1_PORT, GA1_PIN) )
    {
        ga1 = UNCONNECTED;
    }

    if ( ga2 != Chip_GPIO_GetPinState(LPC_GPIO, GA2_PORT, GA2_PIN) )
    {
        ga2 = UNCONNECTED;
    }

    /* Transform the 3-based code in a decimal number */
    index = (9 * ga2) + (3 * ga1) + (1 * ga0);

    if ( index >= IPMBL_TABLE_SIZE )
    {
        return 0;
    }

    return IPMBL_TABLE[index];
}
#undef GPIO_GA_DELAY

