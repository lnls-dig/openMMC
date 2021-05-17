#include "i2c.h"
#include "i2c_mapping.h"
#include "port.h"

i2c_mux_state_t i2c_mux[I2C_MUX_CNT] = {
    { I2C1, -1, 0 },
    { I2C2, -1, 0 }
};

i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT] = {
    [I2C_BUS_UNKNOWN_ID]      = { I2C1, -1, 0 },
    [I2C_BUS_FMC1_ID]         = { I2C1, 0, 1 },
    [I2C_BUS_SFP1_ID]         = { I2C1, 2, 1 },
    [I2C_BUS_SFP2_ID]         = { I2C1, 3, 1 },
    [I2C_BUS_CLOCK_ID]        = { I2C1, 4, 1 },
    [I2C_BUS_PM_ID]           = { I2C1, 5, 1 },
    [I2C_BUS_SENS_ID]         = { I2C1, 6, 1 },
    [I2C_BUS_RTM_ID]          = { I2C1, 7, 1 },
#ifdef MODULE_RTM
    /* MUX on RTM */
    [I2C_BUS_RTM_MEZ0_0_ID]   = {I2C1, 10, 1},
    [I2C_BUS_RTM_MEZ0_1_ID]   = {I2C1, 11, 1},
    [I2C_BUS_RTM_MEZ1_0_ID]   = {I2C1, 12, 1},
    [I2C_BUS_RTM_MEZ1_1_ID]   = {I2C1, 13, 1},
    [I2C_BUS_RTM_SI5324_ID]   = {I2C1, 15, 1},
    [I2C_BUS_RTM_TMP_ID]      = {I2C1, 16, 1},
    [I2C_BUS_RTM_EXAR_ID]     = {I2C1, 17, 1},
#endif
    [I2C_BUS_CPU_ID]          = { I2C1, -1, 1 },
    [I2C_BUS_FPGA_ID]         = { I2C2, -1, 1 }
};

i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT] = {
    [CHIP_ID_MUX_MMC]     = { I2C_BUS_CPU_ID,     0x71 },
    [CHIP_ID_MUX_FPGA]    = { I2C_BUS_FPGA_ID,    0x70 },

    [CHIP_ID_FMC1_EEPROM] = { I2C_BUS_FMC1_ID,    0x50 },

    [CHIP_ID_SFP1]        = { I2C_BUS_SFP1_ID,    0x50 },

    [CHIP_ID_SFP2]        = { I2C_BUS_SFP2_ID,    0x50 },

    [CHIP_ID_SI5324]      = { I2C_BUS_CLOCK_ID,   0x68},

    [CHIP_ID_INA_0]       = { I2C_BUS_PM_ID,      0x40},
    [CHIP_ID_INA_1]       = { I2C_BUS_PM_ID,      0x41},
    [CHIP_ID_XR77129]     = { I2C_BUS_PM_ID,      0x28},

    [CHIP_ID_LM75AIM_0]   = { I2C_BUS_SENS_ID,     0x4B },
    [CHIP_ID_LM75AIM_1]   = { I2C_BUS_SENS_ID,     0x49 },
    [CHIP_ID_LM75AIM_2]   = { I2C_BUS_SENS_ID,     0x4A },
    [CHIP_ID_LM75AIM_3]   = { I2C_BUS_SENS_ID,     0x4F },
    [CHIP_ID_MAX6642]     = { I2C_BUS_SENS_ID,     0x48 },
    [CHIP_ID_EEPROM]      = { I2C_BUS_SENS_ID,     0x51 },
    [CHIP_ID_EEPROM_ID]   = { I2C_BUS_SENS_ID,     0x59 },


#ifdef MODULE_RTM
    [CHIP_ID_RTM_TCA9548] 	     = { I2C_BUS_RTM_ID, 0x76 },
    [CHIP_ID_RTM_EEPROM]  	     = { I2C_BUS_RTM_ID, 0x51 },
    [CHIP_ID_RTM_EEPROM_ID]      = { I2C_BUS_RTM_ID, 0x59 },
    [CHIP_ID_RTM_LM75_0]  	     = { I2C_BUS_RTM_ID, 0x4B },
    [CHIP_ID_RTM_LM75_1]  	     = { I2C_BUS_RTM_ID, 0x4F },
    [CHIP_ID_RTM_MAX6642]        = { I2C_BUS_RTM_ID, 0x4C },
	[CHIP_ID_RTM_PCF8574A] 	     = { I2C_BUS_RTM_ID, 0x3E },

	[CHIP_ID_RTM_MEZ0_EEPROM]	 = { I2C_BUS_RTM_MEZ0_0_ID, 0x50 },
	[CHIP_ID_RTM_MEZ0_EEPROM_ID] = { I2C_BUS_RTM_MEZ0_0_ID, 0x59 },
	[CHIP_ID_RTM_MEZ0_ADT7420]   = { I2C_BUS_RTM_MEZ0_0_ID, 0x48 },

    [CHIP_ID_RTM_MEZ1_EEPROM]    = { I2C_BUS_RTM_MEZ1_0_ID, 0x50 },
    [CHIP_ID_RTM_MEZ1_EEPROM_ID] = { I2C_BUS_RTM_MEZ1_0_ID, 0x59 },
    [CHIP_ID_RTM_MEZ1_ADT7420]   = { I2C_BUS_RTM_MEZ1_0_ID, 0x48 },

	[CHIP_ID_RTM_SI5324]		 = { I2C_BUS_RTM_SI5324_ID, 0x68 },

	[CHIP_ID_RTM_TMP_ADT_1]		 = { I2C_BUS_RTM_TMP_ID, 0x48 },
	[CHIP_ID_RTM_TMP_ADT_2]		 = { I2C_BUS_RTM_TMP_ID, 0x49 },

	[CHIP_ID_RTM_XR77129]		 = { I2C_BUS_RTM_EXAR_ID, 0x28 }
#endif
};

bool i2c_set_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux, int8_t new_state )
{
    portENABLE_INTERRUPTS();

    uint8_t rtm_mux = 0;
    uint8_t rtm_tca_channel;

    if (new_state >= RTM_MUX_BUS_NUM)
    {
    	rtm_mux = 1;
    	rtm_tca_channel = 1 << (new_state - RTM_MUX_BUS_NUM);
    	new_state = i2c_bus_map[I2C_BUS_RTM_ID].mux_bus;
    }

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
#ifdef MODULE_RTM
        if (rtm_mux)
		{
			/* Change RTM MUX state if bus is behind RTM MUX */
			if( xI2CMasterWrite( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_RTM_TCA9548].i2c_address, &rtm_tca_channel, 1 ) != 1 )
			{
				xSemaphoreGive( i2c_mux->semaphore );
				return false;
			}
		}
#endif
    }
    i2c_mux->state = new_state;
    return true;
}

uint8_t i2c_get_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux ){
    if (i2c_mux->i2c_interface == i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface) {
        /* Include enable bit (fourth bit) on channel selection byte */
        uint8_t tca_channel;

        portENABLE_INTERRUPTS();
        /* Read bus state (other master on the bus may have switched it */
        xI2CMasterRead( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX_MMC].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_MUX_MMC].i2c_address, &tca_channel, 1 );

        /* Convert bit position from tca register to actual channel number */
        uint8_t i;
        for (i = 0; i < 8 ; i++) {
        	if (tca_channel & 1 << i) {
        		break;
        	}
        }
        return i;
    }
    else {
        return i2c_mux->state;
    }
}
