#ifdef PORT_INTERRUPTIONS_H_
#error Interruption functions already defined somewhere else! Aborting...
#else
#define PORT_INTERRUPTIONS_H_

#include "core_cm3.h"

#define irq_set_priority(irq, priority) NVIC_SetPriority( irq, priority )
#define irq_enable(irq)                 NVIC_EnableIRQ( irq )
#define irq_disable(irq)                NVIC_DisableIRQ( irq )
#define irq_clear_pending(irq)          NVIC_ClearPendingIRQ( irq )
#define irq_set_pending(irq)            NVIC_SetPendingIRQ( irq )
#define irq_get_active(irq)             NVIC_GetActive( irq )

#endif
