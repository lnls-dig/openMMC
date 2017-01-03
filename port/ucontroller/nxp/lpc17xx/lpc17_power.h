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
 * @file lpc17_power.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Definitions of power mode selection for LPC17xx
 */

#ifdef LPC17_POWER_H_
#undef LPC17_POWER_H_
#endif
#define LPC17_POWER_H_

#include "pmu_17xx_40xx.h"

/**
 * @brief       Enter MCU Sleep mode
 * @return      None
 * @note The sleep mode effects differ between controllers, check the corresponding documentation
 */
#define pm_sleep()              Chip_PMU_Sleep(LPC_PMU, PMU_MCU_SLEEP)

/**
 * @brief       Enter MCU Deep Sleep mode
 * @return      None
 * @note The deep sleep mode effects differ between controllers, check the corresponding documentation
 */
#define pm_deep_sleep()         Chip_PMU_Sleep(LPC_PMU, PMU_MCU_DEEP_SLEEP)

/**
 * @brief       Enter MCU Power Down mode
 * @return      None
 * @note The power down mode effects differ between controllers, check the corresponding documentation
 */
#define pm_power_down()         Chip_PMU_Sleep(LPC_PMU, PMU_MCU_POWER_DOWN)

/**
 * @brief       Enter MCU Deep Power Down mode
 * @return      None
 * @note The deep power down mode effects differ between controllers, check the corresponding documentation
 */
#define pm_deep_power_down()    Chip_PMU_Sleep(LPC_PMU, PMU_MCU_DEEP_PWRDOWN)
