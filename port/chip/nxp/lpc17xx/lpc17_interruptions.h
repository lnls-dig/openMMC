/*
 *   openMMC  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * @file lpc17_interruptions.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief Interruptions control
 */

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
