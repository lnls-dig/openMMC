#include "i2c.h"
#include "i2c_mapping.h"
#include "port.h"

i2c_mux_state_t i2c_mux[I2C_MUX_CNT] = {
    { I2C1, -1, 0 },
    { I2C2, -1, 0 }
};

i2c_bus_mapping_t i2c_bus_map[I2C_BUS_CNT] = {
    [I2C_BUS_UNKNOWN_ID]         = { I2C1, -1, 0 },
    [I2C_BUS_TEMP_SENSORS_ID]    = { I2C2,  0, 1 },
    [I2C_BUS_RTCE_ID]            = { I2C2,  1, 1 },
    [I2C_BUS_PORT2_ID]           = { I2C2,  2, 1 },
    [I2C_BUS_POWER_ID]           = { I2C2,  3, 1 },
    [I2C_BUS_CLOCK_ID]           = { I2C2,  4, 1 },
    [I2C_BUS_RTM_ID]             = { I2C2,  5, 1 },
    [I2C_BUS_FMC2_ID]            = { I2C2,  6, 1 },
    [I2C_BUS_FMC1_ID]            = { I2C2,  7, 1 },
    [I2C_BUS_MUX_ID]             = { I2C2, -1, 1 },
    [I2C_BUS_MCP_ID]             = { I2C1, -1, 1 }
};

i2c_chip_mapping_t i2c_chip_map[I2C_CHIP_CNT] = {
    [CHIP_ID_MUX]         = { I2C_BUS_MUX_ID,    0x70 },
    [CHIP_ID_MAX6642]     = { I2C_BUS_TEMP_SENSORS_ID, 0x48 },
    [CHIP_ID_LM75AIM_0]   = { I2C_BUS_TEMP_SENSORS_ID, 0x4C },
    [CHIP_ID_LM75AIM_1]   = { I2C_BUS_TEMP_SENSORS_ID, 0x4D },
    [CHIP_ID_LM75AIM_2]   = { I2C_BUS_TEMP_SENSORS_ID, 0x4E },
    [CHIP_ID_LM75AIM_3]   = { I2C_BUS_TEMP_SENSORS_ID, 0x4F },


    [CHIP_ID_RTC_EEPROM]  = { I2C_BUS_RTCE_ID,         0x57 },
    [CHIP_ID_SRAM_RTCC]   = { I2C_BUS_RTCE_ID,         0x6F },
    [CHIP_ID_EEPROM]      = { I2C_BUS_RTCE_ID,         0x50 },
    [CHIP_ID_EUI]         = { I2C_BUS_RTCE_ID,         0x58 },
    [CHIP_ID_EEPROM_64K]  = { I2C_BUS_RTCE_ID,         0x51 },

    [CHIP_ID_INA_0]       = { I2C_BUS_POWER_ID,        0x40 },
    [CHIP_ID_INA_1]       = { I2C_BUS_POWER_ID,        0x41 },
    [CHIP_ID_INA_2]       = { I2C_BUS_POWER_ID,        0x42 },

    [CHIP_ID_8V54816]     = { I2C_BUS_CLOCK_ID,        0x5B },

    [CHIP_ID_FMC2_EEPROM] = { I2C_BUS_FMC2_ID,         0x52 },
    [CHIP_ID_FMC2_LM75_0] = { I2C_BUS_FMC2_ID,         0x48 },
    [CHIP_ID_FMC2_LM75_1] = { I2C_BUS_FMC2_ID,         0x49 },

    [CHIP_ID_FMC1_EEPROM] = { I2C_BUS_FMC1_ID,         0x50 },
    [CHIP_ID_FMC1_LM75_0] = { I2C_BUS_FMC1_ID,         0x48 },
    [CHIP_ID_FMC1_LM75_1] = { I2C_BUS_FMC1_ID,         0x49 },

    [CHIP_ID_MCP23016]         = { I2C_BUS_MCP_ID,          0x20 }
};

bool i2c_set_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux, int8_t new_state )
{
    portENABLE_INTERRUPTS();

    if (i2c_mux->i2c_interface == i2c_bus_map[i2c_chip_map[CHIP_ID_MUX].bus_id].i2c_interface) {
        /* Include enable bit (fourth bit) on channel selection byte */
        uint8_t pca_channel = new_state | (1 << 3);

        /* Select desired channel in the I2C switch */
        if( xI2CMasterWrite( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_MUX].i2c_address, &pca_channel, 1 ) != 1 ) {
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
    if (i2c_mux->i2c_interface == i2c_bus_map[i2c_chip_map[CHIP_ID_MUX].bus_id].i2c_interface) {
        /* Include enable bit (fourth bit) on channel selection byte */
        uint8_t pca_channel;

        portENABLE_INTERRUPTS();
        /* Read bus state (other master on the bus may have switched it */
        xI2CMasterRead( i2c_bus_map[i2c_chip_map[CHIP_ID_MUX].bus_id].i2c_interface, i2c_chip_map[CHIP_ID_MUX].i2c_address, &pca_channel, 1 );

        return (pca_channel & 0x07);
    } else {
        return i2c_mux->state;
    }
}
