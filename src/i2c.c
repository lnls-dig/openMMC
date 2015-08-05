/*
 * i2c.c
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

/* Project includes */
#include "i2c.h"
#include "board_defs.h"

/* Project definitions */
#define i2cSTACK_SIZE   ((unsigned short) 300)
#define i2cQUEUE_DEBUG

#define I2CCONSET( id, val )        LPC_I2Cx(id)->CONSET = val
#define I2CCONCLR( id, val )        LPC_I2Cx(id)->CONCLR = val
#define I2CDAT_WRITE( id, val )     LPC_I2Cx(id)->DAT = val
#define I2CDAT_READ( id )           LPC_I2Cx(id)->DAT
#define I2CADDR_WRITE( id, val )    LPC_I2Cx(id)->ADR0 = val
#define I2CADDR_READ( id )          LPC_I2Cx(id)->ADR0
#define I2CMASK( id, val )          LPC_I2Cx(id)->MASK[0] = val

/*static SemaphoreHandle_t pxI2C0_Mutex;
static SemaphoreHandle_t pxI2C1_Mutex;
static SemaphoreHandle_t pxI2C2_Mutex;*/

/* Configuration struct for each I2C interface */
xI2C_Config i2c_cfg[] = {
    {
        .reg = LPC_I2C0,
        .irq = I2C0_IRQn,
        .mode = I2C_Mode_IPMB,
        .pins = {
            .sda_port = I2C0_PORT,
            .sda_pin = I2C0_SDA_PIN,
            .scl_port = I2C0_PORT,
            .scl_pin = I2C0_SCL_PIN,
            .pin_func = I2C0_PIN_FUNC
        },
        .caller_task = NULL,
    },
    {
        .reg = LPC_I2C1,
        .irq = I2C1_IRQn,
        .mode = I2C_Mode_Local_Master,
        .pins = {
            .sda_port = I2C1_PORT,
            .sda_pin = I2C1_SDA_PIN,
            .scl_port = I2C1_PORT,
            .scl_pin = I2C1_SCL_PIN,
            .pin_func = I2C1_PIN_FUNC
        },
        .caller_task = NULL,
    },
    {
        .reg = LPC_I2C2,
        .irq = I2C2_IRQn,
        .mode = I2C_Mode_Local_Master,
        .pins = {
            .sda_port = I2C2_PORT,
            .sda_pin = I2C2_SDA_PIN,
            .scl_port = I2C2_PORT,
            .scl_pin = I2C2_SCL_PIN,
            .pin_func = I2C2_PIN_FUNC
        },
        .caller_task = NULL,
    }
};

/* Function prototypes */
void vI2C_ISR( uint8_t i2c_id );
uint8_t ulCFG_MMC_GA( void );

void I2C0_IRQHandler( void )
{
    vI2C_ISR( I2C0 );
}

void I2C1_IRQHandler( void )
{
    vI2C_ISR( I2C1 );
}

void I2C2_IRQHandler( void )
{
    vI2C_ISR( I2C2 );
}

static uint8_t rx_cnt;
static uint8_t tx_cnt;
static uint8_t rx_data[i2cMAX_MSG_LENGTH];

/* I2C_ISR - I2C interrupt service routine */
void vI2C_ISR( uint8_t i2c_id )
{
    /* Declare local variables */
    portBASE_TYPE xI2CSemaphoreWokeTask;

    /* Initialize variables */
    xI2CSemaphoreWokeTask = pdFALSE;

    /* I2C status handling */
    switch ( LPC_I2Cx( i2c_id )->STAT ){
    case I2C_STAT_START:
    case I2C_STAT_REPEATED_START:
        rx_cnt = 0;
        tx_cnt = 0;
        /* Write Slave Address in the I2C bus, if there's nothing
         * to transmit, the last bit (R/W) will be set to 1
         */
        I2CDAT_WRITE( i2c_id, ( i2c_cfg[i2c_id].msg.addr << 1 ) | ( i2c_cfg[i2c_id].msg.tx_len == 0 ) );
        break;

    case I2C_STAT_SLA_W_SENT_ACK:
        /* Send first data byte */
        I2CDAT_WRITE( i2c_id, *( i2c_cfg[i2c_id].msg.tx_data+tx_cnt ) );
        tx_cnt++;
        I2CCONCLR( i2c_id, I2C_STA );
        break;

    case I2C_STAT_DATA_SENT_ACK:
        /* Transmit the remaining bytes */
        if ( i2c_cfg[i2c_id].msg.tx_len != tx_cnt ){
            I2CDAT_WRITE( i2c_id, *( i2c_cfg[i2c_id].msg.tx_data+tx_cnt ) );
            tx_cnt++;
            I2CCONCLR( i2c_id, I2C_STA );
        } else {
            /* If there's no more data to be transmitted,
             * finish the communication and notify the caller task */
            I2CCONSET( i2c_id, I2C_STO );
            I2CCONCLR( i2c_id, I2C_STA );
            vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].caller_task, &xI2CSemaphoreWokeTask );
        }
        break;

    case I2C_STAT_SLA_R_SENT_ACK:
        /* SLA+R has been transmitted and ACK'd
         * If we want to receive only 1 byte, return NACK on the next byte */
        i2c_cfg[i2c_id].msg.rx_data = rx_data;
        if ( i2c_cfg[i2c_id].msg.rx_len == 1 ){
            I2CCONCLR( i2c_id, ( I2C_STA | I2C_AA ) );
        } else {
            /* If we expect to receive more than 1 byte,
             * return ACK on the next byte */
            I2CCONSET( i2c_id, I2C_AA );
            I2CCONCLR( i2c_id, I2C_STA );
        }
        break;

    case I2C_STAT_DATA_RECV_ACK:
        *(i2c_cfg[i2c_id].msg.rx_data+rx_cnt) = I2CDAT_READ( i2c_id );
        rx_cnt++;
        if (rx_cnt != (i2c_cfg[i2c_id].msg.rx_len) - 1 ){
            I2CCONCLR( i2c_id, I2C_STA );
            I2CCONSET( i2c_id, I2C_AA );
        } else {
            I2CCONCLR( i2c_id, ( I2C_STA | I2C_AA ) );
        }
        break;

    case I2C_STAT_DATA_RECV_NACK:
        *(i2c_cfg[i2c_id].msg.rx_data+rx_cnt) = I2CDAT_READ( i2c_id );
        rx_cnt++;
        I2CCONSET( i2c_id, I2C_STO );
        I2CCONCLR( i2c_id, I2C_STA );
        /* There's no more data to be received */
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].caller_task, &xI2CSemaphoreWokeTask );
        break;

    case I2C_STAT_SLA_R_SENT_NACK:
        I2CCONSET( i2c_id, I2C_STO );
        I2CCONCLR( i2c_id, I2C_STA );
        /* Notify the error ? */
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].caller_task, &xI2CSemaphoreWokeTask );
        break;

        /* Slave Mode */
    case I2C_STAT_SLA_W_RECV_ACK:
    case I2C_STAT_ARB_LOST_SLA_W_RECV_ACK:
        i2c_cfg[i2c_id].msg.i2c_id = i2c_id;
        i2c_cfg[i2c_id].msg.rx_data = rx_data;
        rx_cnt = 0;
        I2CCONSET( i2c_id, I2C_AA );
        break;

    case I2C_STAT_SLA_DATA_RECV_ACK:
        i2c_cfg[i2c_id].msg.rx_data[rx_cnt] = I2CDAT_READ( i2c_id );
        rx_cnt++;
        I2CCONSET ( i2c_id, ( I2C_AA ) );
        break;

    case I2C_STAT_SLA_DATA_RECV_NACK:
        I2CCONCLR ( i2c_id, ( I2C_STA ) );
        I2CCONSET ( i2c_id, ( I2C_AA ) );
        break;

    case I2C_STAT_SLA_STOP_REP_START:
        i2c_cfg[i2c_id].msg.rx_len = rx_cnt;
        if (rx_cnt > 0){
            vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].caller_task, &xI2CSemaphoreWokeTask );
        }
        I2CCONCLR ( i2c_id, ( I2C_STA ) );
        I2CCONSET ( i2c_id, ( I2C_AA ) );
        break;

    default:
        break;
    }
    I2CCONCLR( i2c_id, I2C_SI );

    if (xI2CSemaphoreWokeTask == pdTRUE) {
        portYIELD_FROM_ISR(pdTRUE);
    }
}

/***************
 * vI2C_Init() *
 ***************
 * I2C initialization code called by main
 */
void vI2CInitTask( I2C_ID_T i2c_id, uint32_t uxPriority, I2C_Mode mode )
{
    char pcI2C_Tag[4];

    sprintf( pcI2C_Tag, "I2C%u", i2c_id );
    /* Initialize I2C corresponding pins with the following characteristics:
     *  -> Open Drain
     *  -> Function #3 (If I2C0, it's function #1)
     *  -> No pull-up nor pull-down
     * Configure and init the I2C interruption, with its priority set to one
     * level below the maximum FreeRTOS priority, so the interruption service
     * can access the API and manage the semaphore
     */

    /* TODO: Maybe wrap these functions, or use some board-specific defines
     * so this code is generic enough to be applied on other hardware
     * Example: (if using LPC17xx and LPCOpen library)
     * #define PIN_FUNC_CFG( port, pin, func ) Chip_IOCON_PinMux(...)
    */
    Chip_IOCON_PinMux( LPC_IOCON, i2c_cfg[i2c_id].pins.sda_port, i2c_cfg[i2c_id].pins.sda_pin, IOCON_MODE_INACT, i2c_cfg[i2c_id].pins.pin_func );
    Chip_IOCON_PinMux( LPC_IOCON, i2c_cfg[i2c_id].pins.scl_port, i2c_cfg[i2c_id].pins.scl_pin, IOCON_MODE_INACT, i2c_cfg[i2c_id].pins.pin_func );
    Chip_IOCON_EnableOD( LPC_IOCON, i2c_cfg[i2c_id].pins.sda_port, i2c_cfg[i2c_id].pins.sda_pin );
    Chip_IOCON_EnableOD( LPC_IOCON, i2c_cfg[i2c_id].pins.scl_port, i2c_cfg[i2c_id].pins.scl_pin );
    NVIC_SetPriority(i2c_cfg[i2c_id].irq, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ( i2c_cfg[i2c_id].irq );

    /* Create the ISR semaphore */
    i2c_cfg[i2c_id].isr_smphr = xSemaphoreCreateBinary();
    vQueueAddToRegistry(i2c_cfg[i2c_id].isr_smphr, "I2Cn ISR Semaphore");

    /* Create the I2C driver task */
    TaskHandle_t handle;
    xTaskCreate( vI2CTask, pcI2C_Tag, i2cSTACK_SIZE, NULL, uxPriority, &handle );

    /* Pass the interface number to the task */
    vTaskSetApplicationTaskTag ( handle, (void *)i2c_id );

    /* Set I2C operating mode */
    i2c_cfg[i2c_id].mode = mode;

    /* Create the I2C Mutex that controls access to the cfg struct*/
    i2c_cfg[i2c_id].mutex = xSemaphoreCreateMutex();

    /* Enable and configure I2C clock */
    Chip_I2C_Init( i2c_id );
    Chip_I2C_SetClockRate( i2c_id, 100000 );

    /* Enable I2C interface (Master Mode only) */
    I2CCONSET( i2c_id, I2C_I2EN );

    if ( mode == I2C_Mode_IPMB )
    {
        /* Configure Slave Address */
        uint8_t sla_addr = ulCFG_MMC_GA( );
        I2CADDR_WRITE( i2c_id, sla_addr );

        /* Configure Slave Address Mask */
        I2CMASK( i2c_id, 0xFE);

        /* Enable slave mode */
        I2CCONSET( i2c_id, I2C_AA );
    }

    /* Clear I2C0 interrupt (just in case) */
    I2CCONCLR( i2c_id, I2C_SI );

} /* End of vI2C_Init */

void vI2CTask ( void* pvParameters )
{
    /* Declare local variables */
    uint32_t rx_data[i2cMAX_MSG_LENGTH];
    uint32_t i2c_id = (uint32_t) xTaskGetApplicationTaskTag( NULL );
    uint8_t tx_cnt;
    uint8_t rx_cnt;

#if 0
    /* Variable that stores the actual used stack by this task */
    uint8_t stack = uxTaskGetStackHighWaterMark( xTaskGetCurrentTaskHandle() );
#endif

    for ( ;; ) {
        /* Check the I2C semaphore for a deferred I2C interrupt
         * - proceed only if a the semaphore has been "given"
         * - if semaphore hasn't been given, delay 10 "ticks" and try again
         */
        if ( xSemaphoreTake( i2c_cfg[i2c_id].isr_smphr, (TickType_t) 10 ) )
        {
            switch (LPC_I2Cx(i2c_id)->STAT){
                case I2C_STAT_START:
                case I2C_STAT_REPEATED_START:
                    rx_cnt = 0;
                    tx_cnt = 0;
                    /* Write Slave Address in the I2C bus, if there's nothing
                     * to transmit, the last bit (R/W) will be set to 1
                     */
                    I2CDAT_WRITE( i2c_id, ( i2c_cfg[i2c_id].msg.addr << 1 ) | ( i2c_cfg[i2c_id].msg.tx_len == 0 ) );
                    break;

                case I2C_STAT_SLA_W_SENT_ACK:
                    /* Send first data byte */
                    I2CDAT_WRITE( i2c_id, *( i2c_cfg[i2c_id].msg.tx_data+tx_cnt ) );
                    tx_cnt++;
                    I2CCONCLR( i2c_id, I2C_STA );
                    break;

                case I2C_STAT_DATA_SENT_ACK:
                    /* Transmit the remaining bytes */
                    if ( i2c_cfg[i2c_id].msg.tx_len != tx_cnt ){
                        I2CDAT_WRITE( i2c_id, *( i2c_cfg[i2c_id].msg.tx_data+tx_cnt ) );
                        tx_cnt++;
                        I2CCONCLR( i2c_id, I2C_STA );
                    } else {
                        /* If there's no more data to be transmitted,
                         * finish the communication and notify the caller task */
                        I2CCONSET( i2c_id, I2C_STO );
                        I2CCONCLR( i2c_id, I2C_STA );
                        xTaskNotifyGive( i2c_cfg[i2c_id].caller_task );
                    }
                    break;

                case I2C_STAT_SLA_R_SENT_ACK:
                    /* SLA+R has been transmitted and ACK'd
                     * If we want to receive only 1 byte, return NACK on the next byte */
                    i2c_cfg[i2c_id].msg.rx_data = rx_data;
                    if ( i2c_cfg[i2c_id].msg.rx_len == 1 ){
                        I2CCONCLR( i2c_id, ( I2C_STA | I2C_AA ) );
                    } else {
                        /* If we expect to receive more than 1 byte,
                         * return ACK on the next byte */
                        I2CCONSET( i2c_id, I2C_AA );
                        I2CCONCLR( i2c_id, I2C_STA );
                    }
                    break;

                case I2C_STAT_DATA_RECV_ACK:
                    *(i2c_cfg[i2c_id].msg.rx_data+rx_cnt) = I2CDAT_READ( i2c_id );
                    rx_cnt++;
                    if (rx_cnt != (i2c_cfg[i2c_id].msg.rx_len) - 1 ){
                        I2CCONCLR( i2c_id, I2C_STA );
                        I2CCONSET( i2c_id, I2C_AA );
                    } else {
                        I2CCONCLR( i2c_id, ( I2C_STA | I2C_AA ) );
                    }
                    break;

                case I2C_STAT_DATA_RECV_NACK:
                    *(i2c_cfg[i2c_id].msg.rx_data+rx_cnt) = I2CDAT_READ( i2c_id );
                    rx_cnt++;
                    I2CCONSET( i2c_id, I2C_STO );
                    I2CCONCLR( i2c_id, I2C_STA );
                    /* There's no more data to be received */
                    xTaskNotifyGive( i2c_cfg[i2c_id].caller_task );
                    break;

                case I2C_STAT_SLA_R_SENT_NACK:
                    I2CCONSET( i2c_id, I2C_STO );
                    I2CCONCLR( i2c_id, I2C_STA );
                    /* Notify the error ? */
                    xTaskNotifyGive( i2c_cfg[i2c_id].caller_task );
                    break;

                    /* Slave Mode */
                case I2C_STAT_SLA_W_RECV_ACK:
                case I2C_STAT_ARB_LOST_SLA_W_RECV_ACK:
                    i2c_cfg[i2c_id].msg.i2c_id = i2c_id;
                    i2c_cfg[i2c_id].msg.rx_data = rx_data;
                    rx_cnt = 0;
                    I2CCONSET( i2c_id, I2C_AA );
                    break;

                case I2C_STAT_SLA_DATA_RECV_ACK:
                    i2c_cfg[i2c_id].msg.rx_data[rx_cnt] = I2CDAT_READ( i2c_id );
                    rx_cnt++;
                    I2CCONSET ( i2c_id, ( I2C_AA ) );
                    break;

                case I2C_STAT_SLA_DATA_RECV_NACK:
                    I2CCONCLR ( i2c_id, ( I2C_STA ) );
                    I2CCONSET ( i2c_id, ( I2C_AA ) );
                    break;

                case I2C_STAT_SLA_STOP_REP_START:
                    i2c_cfg[i2c_id].msg.rx_len = rx_cnt;
                    if (rx_cnt > 0)
                    {
                        xTaskNotifyGive( i2c_cfg[i2c_id].caller_task );
                    }
                    I2CCONCLR ( i2c_id, ( I2C_STA ) );
                    I2CCONSET ( i2c_id, ( I2C_AA ) );
                    break;

                default:
                    break;
            }
            I2CCONCLR( i2c_id, I2C_SI );
            NVIC_ClearPendingIRQ( i2c_cfg[i2c_id].irq );
            NVIC_EnableIRQ( i2c_cfg[i2c_id].irq );
        }
    }
}

I2C_err vI2CWrite( I2C_ID_T i2c_id, uint32_t addr, uint32_t * tx_data, uint32_t tx_len )
{
    if (xSemaphoreTake(i2c_cfg[i2c_id].mutex, portMAX_DELAY))
    {
        /* Copy the message to the i2c global struture (controlled by the mutex) */

        /* Maybe use memcpy to pass the tx buffer to the global structure */
        i2c_cfg[i2c_id].msg.i2c_id = i2c_id;
        i2c_cfg[i2c_id].msg.addr = addr;
        i2c_cfg[i2c_id].msg.tx_data = tx_data;
        i2c_cfg[i2c_id].msg.tx_len = tx_len;
        i2c_cfg[i2c_id].msg.rx_data = NULL;
        i2c_cfg[i2c_id].msg.rx_len = 0;

        i2c_cfg[i2c_id].caller_task = xTaskGetCurrentTaskHandle();
        /* Trigger the i2c interruption */
        /* Is it safe to set the flag right now? Won't it stop another ongoing message that is being received for example? */
        I2CCONSET( i2c_id, ( I2C_I2EN | I2C_STA ) );
    }

    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) == pdTRUE )
    {
        /* Include the error in i2c_cfg global structure */
        xSemaphoreGive( i2c_cfg[i2c_id].mutex );
        return i2c_cfg[i2c_id].msg.error;
    }

    /* Should not get here, so return failure */
    xSemaphoreGive( i2c_cfg[i2c_id].mutex );
    return err_FAILURE;
}

I2C_err vI2CRead( I2C_ID_T i2c_id, uint32_t addr, uint32_t * rx_data, uint32_t rx_len )
{
    if (xSemaphoreTake(i2c_cfg[i2c_id].mutex, portMAX_DELAY))
    {
        /* Copy the message to the i2c global struture (controlled by the mutex) */

        /* Maybe use memcpy to pass the tx buffer to the global structure */
        i2c_cfg[i2c_id].msg.i2c_id = i2c_id;
        i2c_cfg[i2c_id].msg.addr = addr;
        i2c_cfg[i2c_id].msg.tx_data = NULL;
        i2c_cfg[i2c_id].msg.tx_len = 0;
        i2c_cfg[i2c_id].msg.rx_data = rx_data;
        i2c_cfg[i2c_id].msg.rx_len = rx_len;

        i2c_cfg[i2c_id].caller_task = xTaskGetCurrentTaskHandle();
        /* Trigger the i2c interruption */
        /* Is it safe to set the flag right now? Won't it stop another ongoing message that is being received for example? */
        I2CCONSET( i2c_id, ( I2C_I2EN | I2C_STA ) );

        /* Wait here until the message is received */
        if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) == pdTRUE )
        {
            /* Debug asserts */
            configASSERT(rx_data);
            configASSERT(i2c_cfg[i2c_id].msg.rx_data);
            /* Copy the received message to the given pointer */
            memcpy (rx_data, i2c_cfg[i2c_id].msg.rx_data, ( i2c_cfg[i2c_id].msg.rx_len * sizeof( uint32_t ) ) );
        }
        xSemaphoreGive( i2c_cfg[i2c_id].mutex );
    }
    return i2c_cfg[i2c_id].msg.error;
}

I2C_err vI2CSlaveTransfer ( I2C_ID_T i2c_id, uint32_t * rx_data )
{
    /* Register this task as the one to be notified when a message comes */
    i2c_cfg[i2c_id].caller_task = xTaskGetCurrentTaskHandle();
    /* Function blocks here until a message is received */
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) == pdTRUE )
    {
        /* Try to take the mutex so we can safely read from the global struct */
        if ( xSemaphoreTake( i2c_cfg[i2c_id].mutex, ( TickType_t ) 1 ) )
        {
            /* Debug asserts */
            configASSERT(rx_data);
            configASSERT(i2c_cfg[i2c_id].msg.rx_data);
            /* Copy the rx buffer to the pointer given */
            memcpy( rx_data, i2c_cfg[i2c_id].msg.rx_data, ( i2c_cfg[i2c_id].msg.rx_len * sizeof( uint32_t ) ) );
            xSemaphoreGive( i2c_cfg[i2c_id].mutex );
        }
    }
    /* Return error code */
    return i2c_cfg[i2c_id].msg.error;
}

/*==============================================================
 * MMC ADDRESSING
 *==============================================================*/
/*
   3.2.1 Geographic Address [2..0] (GA[2..0]) the state of each GA signal is
   represented by G (grounded), U (unconnected), or P (pulled up to Management Power).
   The MMC drives P1 low and reads the GA lines. The MMC then drives P1 high and
   reads the GA lines. Any line that changes state between the two reads indicate
   an unconnected (U) pin.
   The IPMB-L address of a Module can be calculated as (70h + Site Number x 2).
   G = 0, P = 1, U = 2
   GGG 000 0   0x70
   GGP 001 1   0x8A
   GGU 002 2   0x72
   GPG 010 3   0x8E
   GPP 011 4   0x92
   GPU 012 5   0x90
   GUG 020 6   0x74
   GUP 021 7   0x8C
   GUU 022 8   0x76
   PGG 100 9   0x98
   PGP 101 10  0x9C
   PGU 102 11  0x9A
   PPG 110 12  0xA0
   PPP 111 13  0xA4
   PPU 112 14  0x88
   PUG 120 15  0x9E
   PUP 121 16  0x86
   PUU 122 17  0x84
   UGG 200 18  0x78
   UGP 201 19  0x94
   UGU 202 20  0x7A
   UPG 210 21  0x96
   UPP 211 22  0x82
   UPU 212 23  0x80
   UUG 220 24  0x7C
   UUP 221 25  0x7E
   UUU 222 26  0xA2
 */

#define IPMBL_TABLE_SIZE 27

unsigned char IPMBL_TABLE[IPMBL_TABLE_SIZE] = {
    0x70, 0x8A, 0x72, 0x8E, 0x92, 0x90, 0x74, 0x8C, 0x76,
    0x98, 0x9C, 0x9A, 0xA0, 0xA4, 0x88, 0x9E, 0x86, 0x84,
    0x78, 0x94, 0x7A, 0x96, 0x82, 0x80, 0x7C, 0x7E, 0xA2 };

uint8_t ulCFG_MMC_GA( void )
{
    uint8_t ga0, ga1, ga2;
    uint8_t index;

    /* Clar the test pin and read all GA pins */
    Chip_GPIO_SetPinState(LPC_GPIO, GA_PORT, GA_TEST_PIN, 1);
    ga0 = Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA0_PIN);
    ga1 = Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA1_PIN);
    ga2 = Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA2_PIN);

    /* Set the test pin and see if any GA pin has changed is value,
     * meaning that it is unconnected */
    Chip_GPIO_SetPinState(LPC_GPIO, GA_PORT, GA_TEST_PIN, 0);

    if ( ga0 != Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA0_PIN) )
    {
        ga0 = UNCONNECTED;
    }

    if ( ga1 != Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA1_PIN) )
    {
        ga1 = UNCONNECTED;
    }

    if ( ga2 != Chip_GPIO_GetPinState(LPC_GPIO, GA_PORT, GA1_PIN) )
    {
        ga2 = UNCONNECTED;
    }

    /* Transform the 3-based code in a decimal number */
    index = (9 * ga2) + (3 * ga1) + (1 * ga0);

    if ( index >= IPMBL_TABLE_SIZE )
    {
        return 0;
    }

    return IPMBL_TABLE[index];
}
