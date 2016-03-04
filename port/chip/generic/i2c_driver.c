#ifdef USE_GENERIC_I2C_DRIVER

/*! @brief Configuration struct for each I2C interface */
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
    }
};

void vI2C_ISR( uint8_t i2c_id );

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

#define I2C_CON_FLAGS (I2C_AA | I2C_SI | I2C_STO | I2C_STA)


/*! @brief I2C common interrupt service routine
 *
 * I2STAT register is handled inside this function, a state-machine-like implementation for I2C interface.
 *
 * When a full message is trasmitted or received, the task whose handle is written to #i2c_cfg is notified, unblocking it. It also happens when an error occurs.
 * @warning Slave Transmitter mode states are not implemented in this driver and are just ignored.
 */
void vI2C_ISR( uint8_t i2c_id )
{
    /* Declare local variables */
    portBASE_TYPE xI2CSemaphoreWokeTask;

    /* Initialize variables */
    xI2CSemaphoreWokeTask = pdFALSE;
    uint32_t cclr = I2C_CON_FLAGS;

    /* I2C status handling */
    switch ( I2CSTAT( i2c_id ) ){
    case I2C_STAT_START:
    case I2C_STAT_REPEATED_START:
        i2c_cfg[i2c_id].rx_cnt = 0;
        i2c_cfg[i2c_id].tx_cnt = 0;
        /* Write Slave Address in the I2C bus, if there's nothing
         * to transmit, the last bit (R/W) will be set to 1 */
        I2CDAT_WRITE( i2c_id, ( i2c_cfg[i2c_id].msg.addr << 1 ) | ( i2c_cfg[i2c_id].msg.tx_len == 0 ) );
        break;

    case I2C_STAT_SLA_W_SENT_ACK:
        /* Send first data byte */
        I2CDAT_WRITE( i2c_id, i2c_cfg[i2c_id].msg.tx_data[i2c_cfg[i2c_id].tx_cnt] );
        i2c_cfg[i2c_id].tx_cnt++;
        break;

    case I2C_STAT_SLA_W_SENT_NACK:
        cclr &= ~I2C_STO;
        i2c_cfg[i2c_id].msg.error = i2c_err_SLA_W_SENT_NACK;
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].master_task_id, &xI2CSemaphoreWokeTask );
        break;

    case I2C_STAT_DATA_SENT_ACK:
        /* Transmit the remaining bytes */
        if ( i2c_cfg[i2c_id].msg.tx_len != i2c_cfg[i2c_id].tx_cnt ){
            I2CDAT_WRITE( i2c_id, i2c_cfg[i2c_id].msg.tx_data[i2c_cfg[i2c_id].tx_cnt] );
            i2c_cfg[i2c_id].tx_cnt++;
        } else {
            /* If there's no more data to be transmitted,
             * finish the communication and notify the caller task */
            cclr &= ~I2C_STO;
            vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].master_task_id, &xI2CSemaphoreWokeTask );
        }
        break;

    case I2C_STAT_DATA_SENT_NACK:
        cclr &= ~I2C_STO;
        i2c_cfg[i2c_id].msg.error = i2c_err_DATA_SENT_NACK;
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].master_task_id, &xI2CSemaphoreWokeTask );

    case I2C_STAT_SLA_R_SENT_ACK:
        /* SLA+R has been transmitted and ACK'd
         * If we want to receive only 1 byte, return NACK on the next byte */
        if ( i2c_cfg[i2c_id].msg.rx_len > 1 ){
            /* If we expect to receive more than 1 byte,
             * return ACK on the next byte */
            cclr &= ~I2C_AA;
        }
        break;

    case I2C_STAT_DATA_RECV_ACK:
        if ( i2c_cfg[i2c_id].rx_cnt < i2cMAX_MSG_LENGTH - 1 ){
            i2c_cfg[i2c_id].msg.rx_data[i2c_cfg[i2c_id].rx_cnt] = I2CDAT_READ( i2c_id );
            i2c_cfg[i2c_id].rx_cnt++;
            if (i2c_cfg[i2c_id].rx_cnt != (i2c_cfg[i2c_id].msg.rx_len) - 1 ){
                cclr &= ~I2C_AA;
            }
        }
        break;

    case I2C_STAT_DATA_RECV_NACK:
        i2c_cfg[i2c_id].msg.rx_data[i2c_cfg[i2c_id].rx_cnt] = I2CDAT_READ( i2c_id );
        i2c_cfg[i2c_id].rx_cnt++;
        cclr &= ~I2C_STO;
        /* There's no more data to be received */
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].master_task_id, &xI2CSemaphoreWokeTask );
        break;

    case I2C_STAT_SLA_R_SENT_NACK:
        cclr &= ~I2C_STO;
        /* Notify the error */
        i2c_cfg[i2c_id].msg.error = i2c_err_SLA_R_SENT_NACK;
        vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].master_task_id, &xI2CSemaphoreWokeTask );
        break;

        /* Slave Mode */
    case I2C_STAT_SLA_W_RECV_ACK:
    case I2C_STAT_ARB_LOST_SLA_W_RECV_ACK:

        i2c_cfg[i2c_id].msg.i2c_id = i2c_id;
        i2c_cfg[i2c_id].rx_cnt = 0;

        if ( i2c_cfg[i2c_id].mode == I2C_Mode_IPMB ){
            i2c_cfg[i2c_id].msg.rx_data[i2c_cfg[i2c_id].rx_cnt] = I2CADDR_READ(i2c_id);
            cclr &= ~I2C_AA;
            i2c_cfg[i2c_id].rx_cnt++;
        }

        break;

    case I2C_STAT_SLA_DATA_RECV_ACK:
        /* Checks if the buffer is full */
        if ( i2c_cfg[i2c_id].rx_cnt < i2cMAX_MSG_LENGTH ){
            i2c_cfg[i2c_id].msg.rx_data[i2c_cfg[i2c_id].rx_cnt] = I2CDAT_READ( i2c_id );
            i2c_cfg[i2c_id].rx_cnt++;
            cclr &= ~I2C_AA;
        }
        break;

    case I2C_STAT_SLA_DATA_RECV_NACK:
        cclr &= ~I2C_AA;
        i2c_cfg[i2c_id].msg.error = i2c_err_SLA_DATA_RECV_NACK;
        break;

    case I2C_STAT_SLA_STOP_REP_START:
        i2c_cfg[i2c_id].msg.rx_len = i2c_cfg[i2c_id].rx_cnt;
        if (((i2c_cfg[i2c_id].rx_cnt > 0) && (i2c_cfg[i2c_id].mode == I2C_Mode_Local_Master ))) {
            vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].slave_task_id, &xI2CSemaphoreWokeTask );
        }
        if (((i2c_cfg[i2c_id].rx_cnt > 1) && (i2c_cfg[i2c_id].mode == I2C_Mode_IPMB ))) {
            vTaskNotifyGiveFromISR( i2c_cfg[i2c_id].slave_task_id, &xI2CSemaphoreWokeTask );
        }

        cclr &= ~I2C_AA;
        break;

    case I2C_STATUS_BUSERR:
        cclr &= ~I2C_STO;
        break;

    default:
        break;
    }

    if (!(cclr & I2C_CON_STO)) {
        cclr &= ~I2C_CON_AA;

    }
    I2CCONSET(i2c_id, cclr ^ I2C_CON_FLAGS);
    I2CCONCLR(i2c_id, cclr);
    asm("nop");

    if (xI2CSemaphoreWokeTask == pdTRUE) {
        portYIELD_FROM_ISR(pdTRUE);
    }
#endif
