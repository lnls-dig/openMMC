/*
 * ipmi.c
 *
 *   AFCIPMI  --
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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "ipmb.h"
#include "ipmi.h"
#include "board_defs.h"

/* Local variables */
QueueHandle_t ipmi_rxqueue = NULL;

void IPMITask ( void * pvParameters )
{
    static ipmi_msg recv_req;
    for ( ;; ){
        xQueueReceive( ipmi_rxqueue, &recv_req );

        switch ( recv_req.netfn ){

        case NETFN_CHASSIS:
            break;
        case NETFN_BRIDGE:
            break;
        case NETFN_SE:
            break;
        case NETFN_APP:
            break;
        case NETFN_FIRMWARE:
            break;
        case NETFN_STORAGE:
            break;
        case NETFN_TRANSPORT:
            break;
        case NETFN_GRPEXT:
            break;
        case NETFN_CUSTOM:
            break;
        }
    }
}

/* Initializes the IPMI Dispatcher:
 * -> Initializes the IPMB Layer
 * -> Registers the RX queue for incoming requests
 * -> Creates the IPMI task
 */
void ipmi_init ( void )
{
    ipmb_init();
    ipmb_register_rxqueue( &ipmi_rxqueue );
    xTaskCreate( IPMITask, (const char*)"IPMI Dispatcher", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMI_TASK_PRIORITY, ( TaskHandle_t * ) NULL );
}
