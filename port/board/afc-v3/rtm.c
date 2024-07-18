#include "port.h"
#include "rtm_i2c_mapping.h"
#include "eeprom_24xx64.h"
#include "hotswap.h"

void rtm_check_presence( uint8_t *status )
 {
     /* Due to a hardware limitation in the AFC board, we can't rely on reading the PS signal
       since this pin doesn't have a pull-up resistor, it's always read as 0.
       A very dirty workaround is to 'ping' the RTM EEPROM, if it responds, then the board is connected */

    uint8_t dumb;
    *status = HOTSWAP_STATE_URTM_ABSENT;

    if(eeprom_24xx64_read(CHIP_ID_RTM_EEPROM, 0, &dumb, 1, 100)) {
        *status = HOTSWAP_STATE_URTM_PRSENT;
    }
 }

mmc_err rtm_enable_payload_power( void )
{
    gpio_set_pin_state( PIN_PORT(GPIO_EN_RTM_PWR), PIN_NUMBER(GPIO_EN_RTM_PWR), 1 );
    return rtm_enable_payload_power_post();
}

 mmc_err rtm_disable_payload_power( void )
{
    mmc_err error = rtm_disable_payload_power_pre();
    gpio_set_pin_state( PIN_PORT(GPIO_EN_RTM_PWR), PIN_NUMBER(GPIO_EN_RTM_PWR), 0 );
    return error;
}
