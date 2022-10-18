/*
 *   openMMC  --
 *
 *   Copyright (C) 2019  CERN
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
 * @brief Assert function for STM32
 *
 * @author Adam Wujek  <adam.wujek@cern.ch>, CERN
 */

#include "port.h"
#include "task.h"

void stm32_vAssertCalled( char* file, uint32_t line, const char *func)
{
	printf("Assert! file %s, line %d, func %s\r\n", file, (int)line, func);
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
