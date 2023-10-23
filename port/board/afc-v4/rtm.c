#include "i2c_mapping.h"
#include "port.h"
#include "hotswap.h"

void rtm_check_presence( uint8_t *status )
 {
     *status = HOTSWAP_STATE_URTM_ABSENT;

     if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {
         *status = HOTSWAP_STATE_URTM_PRSENT;
     }
 }
