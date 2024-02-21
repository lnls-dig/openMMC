#include "i2c_mapping.h"
#include "port.h"
#include "hotswap.h"
#include "pin_mapping.h"
#include "mcp23016.h"

void rtm_check_presence( uint8_t *status )
 {
     *status = HOTSWAP_STATE_URTM_ABSENT;

     if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {
         *status = HOTSWAP_STATE_URTM_PRSENT;
     }
 }

mmc_err rtm_enable_payload_power( void )
{
    mmc_err err = mcp23016_write_pin(ext_gpios[EXT_GPIO_EN_RTM_PWR].port_num, ext_gpios[EXT_GPIO_EN_RTM_PWR].pin_num, true);
    rtm_enable_payload_power_post();
    return err;
}

mmc_err rtm_disable_payload_power( void )
{
    rtm_disable_payload_power_pre();
    return mcp23016_write_pin(ext_gpios[EXT_GPIO_EN_RTM_PWR].port_num, ext_gpios[EXT_GPIO_EN_RTM_PWR].pin_num, false);
}
