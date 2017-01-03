#include "pin_cfg.h"

void pin_init( void )
{
    uint8_t i;
    uint32_t cfg[] = { PIN_CFG_LIST };
    uint8_t list_len = sizeof(cfg)/(sizeof(cfg[0]));

    for ( i = 0; i < list_len; i++ ) {
        pin_config( PIN_PORT(cfg[i]), PIN_NUMBER(cfg[i]), PIN_FUNC(cfg[i]) );
        if ( PIN_DIR(cfg[i]) != NON_GPIO ) {
            /* Config GPIO direction */
            gpio_set_pin_dir( PIN_PORT(cfg[i]), PIN_NUMBER(cfg[i]), PIN_DIR(cfg[i]));
        }
    }
}
