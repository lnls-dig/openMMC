/*
 * board_version.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Piotr Miedzik <P.Miedzik@gsi.de>
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

#include "FreeRTOS.h"
#include "semphr.h"

#include "utils.h"
#include "board_version.h"
#include "port.h"
#include "ipmb.h"
#include "i2c.h"

/* Chip_ID
 * 0 <- LM75AIM
 * 1 <- LM75AIM
 * 2 <- LM75AIM
 * 3 <- LM75AIM
 * 4 <- MAX6642ATT90
 * 5 <- MCP
 * 6 <- AT24MAC602
 */

struct i2c_chip_mapping {
    uint8_t chip_id;
    uint8_t bus_id;
    uint8_t i2c_address;
};

struct i2c_bus_mapping {
    uint8_t bus_id;
    I2C_ID_T i2c_interface;
    int8_t mux_bus; // AFCv1, AFCv2 mux between P0[0],P0[1] and P0[19]/P[20]
    // AFCv3 MUX IC79 on cpu id
    // AFCv3.1
    // -1 does not uses bus mux
    uint8_t enabled;
};

struct i2c_mux_state {
    I2C_ID_T i2c_interface;
    int8_t state;
    SemaphoreHandle_t semaphore;
};

struct i2c_mux_state i2c_mux[] = {
    { I2C1, -1, 0 },
    { I2C2, -1, 0 },
};

#define I2C_MUX_COUNT (sizeof(i2c_mux) / sizeof(struct i2c_mux_state))

struct i2c_bus_mapping *p_i2c_busmap = NULL;

struct i2c_bus_mapping i2c_bus_map_afc_v2[] = {
    { I2C_BUS_UNKNOWN_ID, I2C1, -1, 0 },
    { I2C_BUS_FMC1_ID,    I2C1,  0, 1 },
    { I2C_BUS_FMC2_ID,    I2C2, -1, 1 },
    { I2C_BUS_CPU_ID,     I2C1,  1, 1 },
    { I2C_BUS_RTM_ID,     I2C1,  0, 0 },
    { I2C_BUS_CLOCK_ID,   I2C1,  0, 1 },
    { I2C_BUS_FPGA_ID,    I2C1,  0, 0 },
};

struct i2c_bus_mapping i2c_bus_map_afc_v3[] = {
    { I2C_BUS_UNKNOWN_ID, I2C1, -1, 0 },
    { I2C_BUS_FMC1_ID,    I2C1,  0, 1 },
    { I2C_BUS_FMC2_ID,    I2C1,  1, 1 },
    { I2C_BUS_CPU_ID,     I2C1, -1, 1 },
    { I2C_BUS_RTM_ID,     I2C1,  3, 1 },
    { I2C_BUS_CLOCK_ID,   I2C1,  2, 1 },
    { I2C_BUS_FPGA_ID,    I2C1,  0, 0 },
};

struct i2c_bus_mapping i2c_bus_map_afc_v3_1[] = {
    { I2C_BUS_UNKNOWN_ID, I2C1, -1, 0 },
    { I2C_BUS_FMC1_ID,    I2C2,  1, 1 },
    { I2C_BUS_FMC2_ID,    I2C2,  0, 1 },
    { I2C_BUS_CPU_ID,     I2C1, -1, 1 },
    { I2C_BUS_RTM_ID,     I2C2,  3, 1 },
    { I2C_BUS_CLOCK_ID,   I2C2,  2, 1 },
    { I2C_BUS_FPGA_ID,    I2C2, -1, 1 },
};

struct i2c_chip_mapping i2c_chip_map[] = {
    {CHIP_ID_MUX      ,     I2C_BUS_CPU_ID,     0x70},
    {CHIP_ID_LM75AIM_0,     I2C_BUS_CPU_ID,     0x4C},
    {CHIP_ID_LM75AIM_1,     I2C_BUS_CPU_ID,     0x4D},
    {CHIP_ID_LM75AIM_2,     I2C_BUS_CPU_ID,     0x4E},
    {CHIP_ID_LM75AIM_3,     I2C_BUS_CPU_ID,     0x4F},
    {CHIP_ID_MAX6642,       I2C_BUS_CPU_ID,     0x48},

    {CHIP_ID_RTC,           I2C_BUS_CPU_ID,     0x9F},
    {CHIP_ID_RTC_EEPROM,    I2C_BUS_CPU_ID,     0x57},
    {CHIP_ID_EEPROM,        I2C_BUS_CPU_ID,     0x50},
    {CHIP_ID_EEPROM_ID,     I2C_BUS_CPU_ID,     0x58},

    {CHIP_ID_INA_0,         I2C_BUS_CPU_ID,     0x40},
    {CHIP_ID_INA_1,         I2C_BUS_CPU_ID,     0x41},
    {CHIP_ID_INA_2,         I2C_BUS_CPU_ID,     0x42},
    {CHIP_ID_INA_3,         I2C_BUS_CPU_ID,     0x43},
    {CHIP_ID_INA_4,         I2C_BUS_CPU_ID,     0x44},
    {CHIP_ID_INA_5,         I2C_BUS_CPU_ID,     0x45},

    {CHIP_ID_ADN,           I2C_BUS_CLOCK_ID,   0x4B},
    {CHIP_ID_SI57x,         I2C_BUS_CLOCK_ID,   0x30},

    {CHIP_ID_EEPROM_FMC1,   I2C_BUS_FMC1_ID,    0x4B},
    {CHIP_ID_EEPROM_FMC2,   I2C_BUS_FMC2_ID,    0x30},
};

#define I2C_CHIP_MAP_COUNT (sizeof(i2c_chip_map)/sizeof(struct i2c_chip_mapping))

manufacturing_info_raw afc_board_info = {0};

void board_i2c_init( void )
{
    uint8_t i;
    for (i=0; i<I2C_MUX_COUNT; i++) {
        i2c_mux[i].semaphore = xSemaphoreCreateBinary();
        vI2CInit(i2c_mux[i].i2c_interface, SPEED_100KHZ, I2C_Mode_Local_Master);
        xSemaphoreGive(i2c_mux[i].semaphore);
    }
}

void board_discover( void )
{
    portENABLE_INTERRUPTS();

    /* Check RTC EEPROM for board info */
//#define WRITE_EEPROM
#ifdef WRITE_EEPROM
    uint8_t unlock[2] = { 0x09, 0x00 };

    /* Carrier type (1-byte):
     * 01h: artix based (AFC)
     * 02h: kintex based (AFCK)
     * ....

     Board version (1-byte):
     * 00h: AFCv1
     * 01h: AFCv2
     * 02h: AFCv3
     * 03h: AFCv3.1

     Manufacturer Private Enterprise Numbers assigned by IANA (3-bytes big-endian)
     * 00h 9Ah 65h (39525): Creotech
     * .....

     Manufacturing day since 1996-01-01 (2-bytes big-endian)
     * 1Bh 1Ch (6940) : 2015-01-01

     Checksum (1-byte) */

    uint8_t ee_write[9] = { 0xF0, 0x01, 0x03, 0x00, 0x9A, 0x65, 0x1B, 0x1C, 0x00 };
    ee_write[8] = calculate_chksum(&ee_write[1], 8);

    /* Unlock protected EEPROM */
    unlock[1] = 0x55;
    xI2CMasterWrite(I2C1, 0x6F, unlock, 2);
    unlock[1] = 0xAA;
    xI2CMasterWrite(I2C1, 0x6F, unlock, 2);
    /* Write data */
    xI2CMasterWrite(I2C1, 0x57, ee_write, sizeof(ee_write));
#endif

    if (xI2CMasterWriteRead(I2C1, 0x57, 0xF0, (uint8_t *) &afc_board_info, sizeof(afc_board_info) ) != sizeof(afc_board_info) ) {
        return;
    }

    portDISABLE_INTERRUPTS();

    uint8_t crc_fail = calculate_chksum((uint8_t *) &afc_board_info, 8 );

    if (crc_fail == 0) {
        if ( ( afc_board_info.carrier_type == CARRIER_TYPE_AFC && afc_board_info.board_version == 0x00 ) ||
             ( afc_board_info.carrier_type == CARRIER_TYPE_AFC && afc_board_info.board_version == 0x01 ) ) {
            i2c_chip_map[CHIP_ID_MUX].bus_id = I2C_BUS_UNKNOWN_ID;
            i2c_chip_map[CHIP_ID_MUX].i2c_address = 0x00;
            p_i2c_busmap = i2c_bus_map_afc_v2;
        } else if ((afc_board_info.carrier_type == CARRIER_TYPE_AFC && afc_board_info.board_version == 0x02) ||
                   afc_board_info.carrier_type == CARRIER_TYPE_AFCK) {
            p_i2c_busmap = i2c_bus_map_afc_v3;
            i2c_chip_map[CHIP_ID_MUX].bus_id = I2C_BUS_CPU_ID;
            i2c_chip_map[CHIP_ID_MUX].i2c_address = 0x70;
        } else if ((afc_board_info.carrier_type == CARRIER_TYPE_AFC && afc_board_info.board_version == 0x03)) {
            p_i2c_busmap = i2c_bus_map_afc_v3_1;
            i2c_chip_map[CHIP_ID_MUX].bus_id = I2C_BUS_FPGA_ID;
            i2c_chip_map[CHIP_ID_MUX].i2c_address = 0x70;
        }
        asm("nop");
    } else {
        //@todo: discover i2c layout if fai;
    }
}

void get_manufacturing_info( manufacturing_info_raw *p_board_info )
{

}

void get_board_type(uint8_t *carrier_type, uint8_t *board_version)
{
    if (carrier_type != NULL) {
        *carrier_type = afc_board_info.carrier_type;
    }

    if (board_version != NULL) {
        *board_version = afc_board_info.board_version;
    }
}


Bool i2c_take_by_busid( uint8_t bus_id, I2C_ID_T * i2c_interface, TickType_t max_wait_time )
{
    uint8_t i;
    struct i2c_mux_state * p_i2c_mux = NULL;
    struct i2c_bus_mapping * p_i2c_bus = &p_i2c_busmap[bus_id];

    I2C_ID_T tmp_interface_id = p_i2c_busmap[bus_id].i2c_interface;
    for (i=0; i<I2C_MUX_COUNT; i++) {
        if (i2c_mux[i].i2c_interface == tmp_interface_id) {
            p_i2c_mux = &i2c_mux[i];
            break;
        }
    }

    if (p_i2c_mux == NULL) {
        return false;
    }
    if (p_i2c_bus->enabled == 0) {
        return false;
    }
    if (p_i2c_mux->semaphore == 0) {
        return false;
    }

    /* Try to take the semaphore to win the bus */
    if (xSemaphoreTake(p_i2c_mux->semaphore, max_wait_time) == pdFALSE) {
        return false;
    }

    /* This bus is not multiplexed, no action needed */
    if (p_i2c_bus->mux_bus == -1) {
        *i2c_interface = p_i2c_mux->i2c_interface;
        return true;
    }

    /* This bus mux is in correct state */
    if (p_i2c_mux->state == p_i2c_bus->mux_bus) {
        *i2c_interface = p_i2c_mux->i2c_interface;
        return true;
    } else if (i2c_chip_map[CHIP_ID_MUX].bus_id == I2C_BUS_UNKNOWN_ID) {
        /* Change bus mux */
        if (p_i2c_bus->mux_bus == 0) {
            /*! @todo Change LPC specific functions for generic macros */
            Chip_I2C_Disable(p_i2c_bus->i2c_interface);
            Chip_I2C_DeInit(p_i2c_bus->i2c_interface);

            Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC0);
            Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC0);
            //Chip_IOCON_EnableOD(LPC_IOCON, 0,  0);
            //Chip_IOCON_EnableOD(LPC_IOCON, 0,  1);

            Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC3);
            Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC3);
            Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
            Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);
            Chip_I2C_Init(p_i2c_bus->i2c_interface);
            Chip_I2C_Enable(p_i2c_bus->i2c_interface);

        } else if ((p_i2c_bus->mux_bus == 1)) {
            Chip_I2C_Disable(p_i2c_bus->i2c_interface);

            Chip_I2C_DeInit(p_i2c_bus->i2c_interface);

            Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC3);
            Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC3);
            Chip_IOCON_EnableOD(LPC_IOCON, 0, 0);
            Chip_IOCON_EnableOD(LPC_IOCON, 0, 1);

            Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC0);
            Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC0);

            Chip_I2C_Init(p_i2c_bus->i2c_interface);
            Chip_I2C_Enable(p_i2c_bus->i2c_interface);

        } else {
            xSemaphoreGive(p_i2c_mux->semaphore);
            return false;
        }
        p_i2c_mux->state = p_i2c_bus->mux_bus;
        *i2c_interface = p_i2c_mux->i2c_interface;
        return true;
        // this is mux inside MMC
    } else {
        while (xI2CMasterWrite(i2c_chip_map[CHIP_ID_MUX].bus_id, i2c_chip_map[CHIP_ID_MUX].i2c_address, (uint8_t *)&p_i2c_bus->mux_bus, 1 ) < 1) { }
        p_i2c_mux->state = p_i2c_bus->mux_bus;
        *i2c_interface = p_i2c_mux->i2c_interface;
        return true;
    }

    xSemaphoreGive(p_i2c_mux->semaphore);
    return false;
}

Bool i2c_take_by_chipid(uint8_t chip_id, uint8_t * i2c_address, I2C_ID_T * i2c_interface,  TickType_t max_wait_time)
{
    if (chip_id > I2C_CHIP_MAP_COUNT) {
        return false;
    }

    uint8_t bus_id = i2c_chip_map[chip_id].bus_id;
    if (i2c_address != NULL) {
        *i2c_address = i2c_chip_map[chip_id].i2c_address;
    }

    return i2c_take_by_busid(bus_id, i2c_interface, max_wait_time);
}

void i2c_give(I2C_ID_T i2c_interface)
{
    for (uint8_t i=0; i<I2C_MUX_COUNT; i++) {
        if (i2c_mux[i].i2c_interface == i2c_interface) {
            xSemaphoreGive(i2c_mux[i].semaphore);
            break;
        }
    }
}
