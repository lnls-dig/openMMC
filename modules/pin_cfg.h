#ifndef PIN_CFG_H_
#define PIN_CFG_H_

#include "port.h"

/* 32 bit value in the format -> [port][pin][func][dir] each field [] is one byte */
#define PIN_DEF( port, pin, func, dir ) ( (port << 24) | (pin << 16) | (func << 8) | dir )

#define PIN_PORT( pin_def )      ((pin_def & 0xFF000000) >> 24)
#define PIN_NUMBER( pin_def )    ((pin_def & 0x00FF0000) >> 16)
#define PIN_FUNC( pin_def )      ((pin_def & 0x0000FF00) >> 8)
#define PIN_DIR( pin_def )       ((pin_def & 0x000000FF) >> 0)

/* For other mcus like Atmel's it should be PORTA, PORTB, etc */
#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
#define PORT4 4

#define NON_GPIO 0xFF

void pin_init( void );

#endif
