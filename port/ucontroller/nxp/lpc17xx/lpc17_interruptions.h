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

/**
 * @file lpc17_interruptions.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Interruptions control
 */

#ifdef PORT_INTERRUPTIONS_H_
#error Interruption functions already defined somewhere else! Aborting...
#else
#define PORT_INTERRUPTIONS_H_

#include "core_cm3.h"

/**
 * @brief Set interruption priority
 *
 * @param [in] irq         Interrupt number
 * @param [in] priority    Priority level
 */
#define irq_set_priority(irq, priority) NVIC_SetPriority( irq, priority )

/**
 * @brief Enable interruption
 *
 * @param [in] irq         Interrupt number
 */
#define irq_enable(irq)                 NVIC_EnableIRQ( irq )

/**
 * @brief Disable interruption
 *
 * @param [in] irq         Interrupt number
 */
#define irq_disable(irq)                NVIC_DisableIRQ( irq )

/**
 * @brief  Clear Pending Interrupt
 *
 * The function clears the pending bit of an external interrupt.
 *
 * @param [in] irq  External interrupt number. Value cannot be negative.
 */
#define irq_clear_pending(irq)          NVIC_ClearPendingIRQ( irq )

/**
 * @brief  Set Pending Interrupt
 *
 * The function sets the pending bit of an external interrupt.
 *
 * @param [in] irq  External interrupt number. Value cannot be negative.
 */
#define irq_set_pending(irq)            NVIC_SetPendingIRQ( irq )

/**
 * @brief  Get Active Interrupt
 *
 * The function reads the interruption status register and returns the active bit.
 *
 * @param [in] irq  Interrupt number.
 *
 * @return 0  Interrupt status is not active.
 * @return 1  Interrupt status is active.
 */
#define irq_get_active(irq)             NVIC_GetActive( irq )

#endif
