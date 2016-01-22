#ifdef WATCHDOG_H_
#undef WATCHDOG_H_
#else
#define WATCHDOG_H_

void watchdog_init( void );
void WatchdogTask( void * Parameters );
void watchdog_reset_mcu( void );

#endif
