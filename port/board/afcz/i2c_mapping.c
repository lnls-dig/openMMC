#include "i2c.h"
#include "i2c_mapping.h"
#include "port.h"


i2c_mux_state_t i2c_mux[I2C_MUX_CNT] = {
    { I2C1, -1, 0 },
    { I2C2, -1, 0 }
};

i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT] = {
    [I2C_BUS_UNKNOWN_ID]    = { I2C1, -1, 0 },
    [I2C_BUS_FMC1_ID]       = { I2C1,  0, 1 },
    [I2C_BUS_FMC2_ID]       = { I2C1,  1, 1 },
    [I2C_BUS_CLOCKS_ID]     = { I2C1,  2, 1 },
    [I2C_BUS_EXAR_ID]       = { I2C1,  4, 1 },
    [I2C_BUS_PM_ID]         = { I2C1,  5, 1 },
    [I2C_BUS_SENS_ID]       = { I2C1,  6, 1 },
    [I2C_BUS_RTM_ID]        = { I2C1,  7, 1 },

    [I2C_BUS_CPU_ID]        = { I2C1, -1, 1 },
    [I2C_BUS_FPGA_ID]       = { I2C2, -1, 1 },
    [I2C_BUS_PM_GPIO_ID]    = { I2C1, -1, 1 }
};

i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT] = {
    [CHIP_ID_MUX_MMC]     = { I2C_BUS_CPU_ID,     0x71 },
	[CHIP_ID_MUX_FPGA]    = { I2C_BUS_FPGA_ID,    0x70 },

    [CHIP_ID_FMC1_EEPROM] = { I2C_BUS_FMC1_ID,    0x50 },
	[CHIP_ID_FMC2_EEPROM] = { I2C_BUS_FMC2_ID,    0x50 },

	[CHIP_ID_SI5341]      = { I2C_BUS_CLOCKS_ID,  0x76 },
	[CHIP_ID_TCA9539_CLK] = { I2C_BUS_CLOCKS_ID,  0x74 },
	[CHIP_ID_8V54816]     = { I2C_BUS_CLOCKS_ID,  0x58 },

	[CHIP_ID_XR7724_2]    = { I2C_BUS_EXAR_ID,    0x28 },

    [CHIP_ID_INA3221]     = { I2C_BUS_PM_ID,      0x40},
    [CHIP_ID_XR7724_1]    = { I2C_BUS_PM_ID,      0x28},

    [CHIP_ID_LM75AIM_0]   = { I2C_BUS_SENS_ID,    0x4B },
    [CHIP_ID_LM75AIM_1]   = { I2C_BUS_SENS_ID,    0x49 },
    [CHIP_ID_LM75AIM_2]   = { I2C_BUS_SENS_ID,    0x4A },
    [CHIP_ID_LM75AIM_3]   = { I2C_BUS_SENS_ID,    0x4F },
    [CHIP_ID_MAX6642]     = { I2C_BUS_SENS_ID,    0x48 },
    [CHIP_ID_EEPROM]      = { I2C_BUS_SENS_ID,    0x51 },
    [CHIP_ID_EEPROM_ID]   = { I2C_BUS_SENS_ID,    0x59 },
    [CHIP_ID_EEPROM_64K]  = { I2C_BUS_SENS_ID,    0x54 },

#ifdef MODULE_RTM
    [CHIP_ID_RTM_LM75_0]    = { I2C_BUS_RTM_ID,    0x4A },
    [CHIP_ID_RTM_LM75_1]    = { I2C_BUS_RTM_ID,    0x4F },
    [CHIP_ID_RTM_LM75_2]    = { I2C_BUS_RTM_ID,    0x4B },
	[CHIP_ID_RTM_PCA9554]   = { I2C_BUS_RTM_ID,    0x20 },
	[CHIP_ID_RTM_EEPROM]    = { I2C_BUS_RTM_ID,    0x51 },
	[CHIP_ID_RTM_EEPROM_ID] = { I2C_BUS_RTM_ID,    0x59 },
#endif

    [CHIP_ID_TCA9539_PM]  = { I2C_BUS_PM_GPIO_ID, 0x75 }
};

bool i2c_set_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux, int8_t new_state )
{
    portENABLE_INTERRUPTS();

    if (i2c_mux->i2c_interface == i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface)
    {
        /* Include enable bit (fourth bit) on channel selection byte */
        uint8_t tca_channel = 1 << new_state;

        /* Select desired channel in the I2C switch */
        if(xI2CMasterWrite( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_MUX_MMC].i2c_address, &tca_channel, 1 ) != 1 )
        {
        	/* We failed to configure the I2C Mux, release the semaphore */
            xSemaphoreGive( i2c_mux->semaphore );
            return false;
        }
    }

    i2c_mux->state = new_state;
    return true;
}

uint8_t i2c_get_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux )
{
    if (i2c_mux->i2c_interface == i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface) {
        /* Include enable bit (fourth bit) on channel selection byte */
        uint8_t tca_channel;

        portENABLE_INTERRUPTS();
        /* Read bus state (other master on the bus may have switched it */
        xI2CMasterRead( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_MUX_MMC].i2c_address, &tca_channel, 1 );

        /* Convert bit position from tca register to actual channel number */
        uint8_t i;
        for (i = 0; i < 8 ; i++)
        {
        	if (tca_channel & 1 << i)
        		break;
        }
        return i;
    } else {
        return i2c_mux->state;
    }
}
