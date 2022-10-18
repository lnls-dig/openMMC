/*
 *   openMMC  --
 *
 *   Copyright (C) 2019 CERN
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
 * @file stm32_i2c.c
 * @author Adam Wujek <adam.wujek@cern.ch>, CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>, CERN
 * @date 2019-2021
 *
 * @brief I2C driver for STM32F30x
 */


#include "port.h"
#include "string.h"
#include "i2c_mapping.h"
#include "stm32f30x.h"
#include "stm32f30x_i2c.h"
#include "task.h"
#include "stm32_ring_buffer.h"

#include <errno.h>

#define I2C_RX_BUF_SIZE 64
#define I2C_RX_QUEUE_SIZE 512

struct stm32_i2c_iface
{
    SemaphoreHandle_t lock;
    I2C_ID_T id_mmc;
    uint8_t is_slave;
    I2C_TypeDef *base;
    uint8_t *rx_buf;
    int rx_buf_pos;
    struct stm32_ring_buffer rx_queue;
    int addr_count, rx_count, overflow_count;
    int cnt;
};

#ifdef DEBUG
#define I2C_DBG(...) printf(__VA_ARGS__);
#else
#define I2C_DBG(...)
#endif

struct stm32_i2c_iface stm_i2c_ifs[I2C_NUM_INTERFACE];

static struct stm32_i2c_iface *id2iface(I2C_ID_T id)
{
    if (id >= I2C_NUM_INTERFACE) {
        I2C_DBG("i2c if not found\n\r");
        for(;;);
    }
    return &stm_i2c_ifs[id];
}

static int i2c_enable_irqs(struct stm32_i2c_iface *iface, int enable)
{
    I2C_ITConfig(iface->base, I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ERRI, enable ? ENABLE : DISABLE);
    return 0;
}

static void i2c_common_init(struct stm32_i2c_iface *iface, uint8_t own_address)
{
#define I2C_TIMINGR_VALUE 0x00902025
    I2C_InitTypeDef I2C_InitStructure;
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0x00;
    I2C_InitStructure.I2C_Timing = I2C_TIMINGR_VALUE;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    // I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = own_address;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    // I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Init(iface->base, &I2C_InitStructure);
}

static void i2c_low_level_init(I2C_ID_T id_mmc, uint8_t own_address)
{
    struct stm32_i2c_iface *iface = id2iface(id_mmc);
    switch (id_mmc)
    {
    case I2C1_ID:
        iface->base = I2C1;
        RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        break;

    case I2C2_ID:
        iface->base = I2C2;
        RCC_I2CCLKConfig(RCC_I2C2CLK_HSI);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        break;

    case I2C3_ID:
        iface->base = I2C3;
        RCC_I2CCLKConfig(RCC_I2C3CLK_HSI);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
        break;
    }

    iface->lock = xSemaphoreCreateMutex();
    i2c_common_init(iface, own_address);

    iface->is_slave = 0;
}

void i2c_low_level_slave_init(I2C_ID_T id_mmc, uint8_t own_address)
{
    struct stm32_i2c_iface *iface = id2iface(id_mmc);
    IRQn_Type event_irq_num, err_irq_num;
    switch (id_mmc)
    {
    case I2C1_ID:
        event_irq_num = I2C1_EV_IRQn;
        err_irq_num = I2C1_ER_IRQn;
        break;

    case I2C2_ID:
        event_irq_num = I2C2_EV_IRQn;
        err_irq_num = I2C2_ER_IRQn;
        break;

    case I2C3_ID:
        event_irq_num = I2C3_EV_IRQn;
        err_irq_num = I2C3_ER_IRQn;
        break;
    }
    stm32_rbuf_init((struct stm32_ring_buffer *)&iface->rx_queue, I2C_RX_QUEUE_SIZE);
    iface->rx_buf = pvPortMalloc(I2C_RX_BUF_SIZE);
    iface->rx_buf_pos = 0;
    I2C_StretchClockCmd(iface->base, DISABLE);
    I2C_SlaveByteControlCmd(iface->base, DISABLE);
    i2c_common_init(iface, own_address);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = event_irq_num;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C error interrupt -------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = err_irq_num;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable I2C event and buffer interrupts */
    i2c_enable_irqs(iface, 1);

    iface->is_slave = 1;
}

static int i2c_wait_flag(struct stm32_i2c_iface *iface, uint32_t mask, int state, int timeout)
{
    do
    {
        if (I2C_GetFlagStatus(iface->base, mask) == state)
            return 0;

    } while (timeout--);

    return -ETIMEDOUT;
}

static int i2c_master_write(struct stm32_i2c_iface *iface, uint8_t address, int count, const uint8_t *data, int timeout)
{
    int rv;
    if ((rv = i2c_wait_flag(iface, I2C_ISR_BUSY, RESET, timeout)) < 0)
    {
        I2C_DBG("master_write: busy tmo\n\r");
        return rv;
    }

    I2C_TransferHandling(iface->base, address, count, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);

    if ((rv = i2c_wait_flag(iface, I2C_ISR_TXIS, SET, timeout)) < 0)
    {
        I2C_DBG("master_write: start tmo\n\r");
        return rv;
    }

    int i;

    for (i = 0; i < count; i++)
    {
        I2C_SendData(iface->base, (uint8_t)data[i]);

        if (i == count - 1)
            break;

        if ((rv = i2c_wait_flag(iface, I2C_ISR_TXIS, SET, timeout)) < 0)
        {
            I2C_DBG("master_write: send tmo\n\r");
            return rv;
        }
    }

    if ((rv = i2c_wait_flag(iface, I2C_ISR_STOPF, SET, timeout)) < 0)
    {
        I2C_DBG("master_write: stop tmo\n\r");
        return rv;
    }

    /* Clear STOPF flag */
    I2C_ClearFlag(iface->base, I2C_ICR_STOPCF);

    return count;
}

static int i2c_master_write_read(struct stm32_i2c_iface *iface, uint8_t address, int tx_count, const uint8_t *tx_data, int rx_count, uint8_t *rx_data, int timeout)
{
    int rv;
    if ((rv = i2c_wait_flag(iface, I2C_ISR_BUSY, RESET, timeout)) < 0)
        return rv;

    I2C_TransferHandling(iface->base, address, tx_count, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

    if ((rv = i2c_wait_flag(iface, I2C_ISR_TXIS, SET, timeout)) < 0)
        return rv;

    int i;

    for (i = 0; i < tx_count; i++)
    {
        I2C_SendData(iface->base, (uint8_t)tx_data[i]);

        if (i == tx_count - 1)
            break;

        if ((rv = i2c_wait_flag(iface, I2C_ISR_TXIS, SET, timeout)) < 0)
            return rv;
    }

    if ((rv = i2c_wait_flag(iface, I2C_ISR_TC, SET, timeout)) < 0)
        return rv;

    I2C_TransferHandling(iface->base, address, rx_count, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

    for (i = 0; i < rx_count; i++)
    {
        if ((rv = i2c_wait_flag(iface, I2C_ISR_RXNE, SET, timeout)) < 0)
            return rv;

        rx_data[i] = I2C_ReceiveData(iface->base);
    }

    if ((rv = i2c_wait_flag(iface, I2C_ISR_STOPF, SET, timeout)) < 0)
        return rv;

    /* Clear STOPF flag */
    I2C_ClearFlag(iface->base, I2C_ICR_STOPCF);
    return 0;
}

static int i2c_master_read(struct stm32_i2c_iface *iface, uint8_t address, int rx_count, uint8_t *rx_data, int timeout)
{
    int rv;
    if ((rv = i2c_wait_flag(iface, I2C_ISR_BUSY, RESET, timeout)) < 0)
        return rv;

    I2C_TransferHandling(iface->base, address, rx_count, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

    for (int i = 0; i < rx_count; i++)
    {
        if ((rv = i2c_wait_flag(iface, I2C_ISR_RXNE, SET, timeout)) < 0)
            return rv;

        rx_data[i] = I2C_ReceiveData(iface->base);
    }

    if ((rv = i2c_wait_flag(iface, I2C_ISR_STOPF, SET, timeout)) < 0)
        return rv;

    /* Clear STOPF flag */
    I2C_ClearFlag(iface->base, I2C_ICR_STOPCF);
    return rx_count;
}

static void i2c_irq_handler(volatile struct stm32_i2c_iface *iface)
{
    if (I2C_GetITStatus(iface->base, I2C_ISR_ADDR))
    {
        iface->rx_buf_pos = 0;
        iface->addr_count++;
        I2C_ClearITPendingBit(iface->base, I2C_ISR_ADDR);
    }

    if (I2C_GetITStatus(iface->base, I2C_ISR_STOPF))
    {
        if (iface->rx_buf_pos > 0)
        {
            int nfree = stm32_rbuf_avail((struct stm32_ring_buffer *)&iface->rx_queue);
            if (nfree >= iface->rx_buf_pos + 1)
            {
                stm32_rbuf_put((struct stm32_ring_buffer *)&iface->rx_queue, iface->rx_buf_pos, 1);
                for (int i = 0; i < iface->rx_buf_pos; i++)
                {
                    stm32_rbuf_put((struct stm32_ring_buffer *)&iface->rx_queue, iface->rx_buf[i], 1);
                }

                iface->rx_buf_pos = 0;
                iface->rx_count++;
            }
            else
            {
                iface->overflow_count++;
            }
        }
        I2C_ClearITPendingBit(iface->base, I2C_ISR_STOPF);
    }

    if (I2C_GetITStatus(iface->base, I2C_ISR_RXNE))
    {
        volatile int d = I2C_ReceiveData(iface->base);
        iface->cnt++;

        if (iface->rx_buf_pos >= I2C_RX_BUF_SIZE)
        {
            iface->overflow_count++;
        }
        else
        {
            iface->rx_buf[iface->rx_buf_pos] = d;
            iface->rx_buf_pos++;
        }

        I2C_ClearITPendingBit(iface->base, I2C_ISR_RXNE);
    }
}

uint32_t I2C1_EV_IRQHandler(void)
{
    i2c_irq_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C1_ID]);
    return 0;
}

uint32_t I2C2_EV_IRQHandler(void)
{
    i2c_irq_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C2_ID]);
    return 0;
}

uint32_t I2C3_EV_IRQHandler(void)
{
    i2c_irq_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C3_ID]);
    return 0;
}

static void i2c_error_handler(volatile struct stm32_i2c_iface *iface)
{
    iface->rx_buf_pos = 0;
    I2C_ClearITPendingBit(iface->base, 0xff00);
}

uint32_t I2C1_ER_IRQHandler(void)
{
    i2c_error_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C1_ID]);
    return 0;
}

uint32_t I2C2_ER_IRQHandler(void)
{
    i2c_error_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C2_ID]);
    return 0;
}

uint32_t I2C3_ER_IRQHandler(void)
{
    i2c_error_handler((volatile struct stm32_i2c_iface *)&stm_i2c_ifs[I2C3_ID]);
    return 0;
}

static int i2c_slave_read(struct stm32_i2c_iface *iface, uint8_t *rx_data, int rx_size)
{
    int n = stm32_rbuf_count(&iface->rx_queue);
    if (n < 1)
        return 0;

    int psize = stm32_rbuf_tail(&iface->rx_queue, 0);

    if (psize > n)
        return -ENOMEM;

    if (psize > n + 1)
        return 0;

    stm32_rbuf_get(&iface->rx_queue, 0);

    int i;
    for (i = 0; i < psize; i++)
        rx_data[i] = stm32_rbuf_get(&iface->rx_queue, 0);

    return psize;
}

void vI2CConfig(I2C_ID_T id, uint32_t speed)
{
    i2c_low_level_init(id, 0);
}

void vI2CSlaveSetup(I2C_ID_T id, uint8_t slave_addr)
{
    I2C_DBG("I2C Slave @ 0x%x\n", slave_addr);
    i2c_low_level_slave_init(id, slave_addr << 1);
}

int xI2CMasterWrite(I2C_ID_T id, uint8_t addr, uint8_t *tx_buff, uint8_t tx_len)
{
    struct stm32_i2c_iface *iface = id2iface(id);

    while (xSemaphoreTake(iface->lock, (TickType_t)1) != pdTRUE)
        ;

    if (iface->is_slave)
        i2c_enable_irqs(iface, 0);

    int rv = i2c_master_write(iface, addr << 1, tx_len, tx_buff, 10000);

    if (iface->is_slave)
        i2c_enable_irqs(iface, 1);

    I2C_DBG("I2C%d: [slave %d] wrote %d bytes to 0x%x [ret %d]\n\r", id, iface->is_slave, tx_len, addr, rv);

    xSemaphoreGive(iface->lock);

    return rv;
}

int xI2CMasterRead(I2C_ID_T id, uint8_t addr, uint8_t *rx_buff, uint8_t rx_len)
{
    struct stm32_i2c_iface *iface = id2iface(id);

    while (xSemaphoreTake(iface->lock, (TickType_t)1) != pdTRUE)
        ;

    if (iface->is_slave)
        i2c_enable_irqs(iface, 0);

    int rv = i2c_master_read(iface, addr << 1, rx_len, rx_buff, 1000);

    if (iface->is_slave)
        i2c_enable_irqs(iface, 1);

    I2C_DBG("I2C%d: read %d bytes from 0x%x [ret %d]\n\r", iface->id_mmc, rx_len, addr, rv);

    xSemaphoreGive(iface->lock);

    return rv;
}

int xI2CMasterWriteRead(I2C_ID_T id, uint8_t addr, uint8_t cmd, uint8_t *rx_buff, uint8_t rx_len)
{
    struct stm32_i2c_iface *iface = id2iface(id);

    while (xSemaphoreTake(iface->lock, (TickType_t)1) != pdTRUE)
        ;

    if (iface->is_slave)
        i2c_enable_irqs(iface, 0);

    int rv = i2c_master_write_read(iface, addr << 1, 1, &cmd, rx_len, rx_buff, 1000);

    if (iface->is_slave)
        i2c_enable_irqs(iface, 1);

    xSemaphoreGive(iface->lock);

    return rv;
}

uint8_t xI2CSlaveReceive(I2C_ID_T id, uint8_t *rx_buff, uint8_t buff_len, uint32_t timeout)
{
    struct stm32_i2c_iface *iface = id2iface(id);

    while (xSemaphoreTake(iface->lock, (TickType_t)1) != pdTRUE)
        ;
    
    int rv = i2c_slave_read(iface, rx_buff, buff_len);

    if (rv > 0)
    {
        int i;
        I2C_DBG("I2C%d: slave-read %d bytes\n\r", iface->id_mmc, rv);
        for (i = 0; i < rv; i++)
            I2C_DBG("%02x ", rx_buff[i]);
        I2C_DBG("\n\r");

        xSemaphoreGive(iface->lock);

        return rv;
    }
    else
    {
        xSemaphoreGive(iface->lock);
        vTaskDelay(1);
        return 0;
    }
}
