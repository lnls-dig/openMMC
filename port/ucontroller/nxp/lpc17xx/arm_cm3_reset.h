#include "core_cm3.h"

static inline void mcu_reset(void)
{
    /* Issue a system reset */
    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
                  (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                  SCB_AIRCR_SYSRESETREQ_Msk);
    /* Ensure completion of memory accesses */
    __asm volatile ("dsb");
    /* Wait for the reset */
    while(1);
}
