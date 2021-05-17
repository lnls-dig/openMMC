#include "port.h"
#include "custom.h"

void custom_init(void)
{
    // make sure address is valid
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR1), PIN_NUMBER(GPIO_I2C_MUX_ADDR1), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR2), PIN_NUMBER(GPIO_I2C_MUX_ADDR2), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_SW_RESETn), PIN_NUMBER(GPIO_SW_RESETn), GPIO_LEVEL_HIGH);

    // I2C FIX for 1776
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));

    // Enable RTM P3V3_MP
    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_MP), PIN_NUMBER(GPIO_EN_RTM_MP), true);
    }
}
