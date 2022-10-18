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
 * @file stm32_uart.c
 * @author Adam Wujek, <adam.wujek@cern.ch>
 * @date March 2019
 *
 * @brief
 */

#include "port.h"
#include "task.h"
#include "stm32_ring_buffer.h"

#define OVERRUN_UART  1
#define OVERRUN_RBUFF 2

#define UART_BUFFER_SIZE 512

struct stm32_uart_device_priv
{
    struct stm32_ring_buffer rx_buf;
    struct stm32_ring_buffer tx_buf;
};

void uart_init ( uint8_t id )
{
  struct stm32_uart_cfg *cfg = &usart_cfg[id];

  cfg->priv = pvPortMalloc( sizeof( struct stm32_uart_device_priv ) );

  /* Enable GPIO & USART clock */
  (*(cfg->periph_func))(cfg->periph, ENABLE);

//  RCC_AHBPeriphClockCmd( get_uart_gpio_port_ahb_id( cfg->port_TX ), ENABLE);
  //RCC_AHBPeriphClockCmd( get_uart_gpio_port_ahb_id( cfg->port_RX ), ENABLE);

  /* Configure RX if enabled */

  USART_Init(cfg->periph_base, &cfg->USART_InitStructure);

  if( cfg->irq_id >= 0 )
  {
    if( cfg->flags & STM32_UART_ENABLE_RX_IRQ )
    {
      stm32_rbuf_init( &cfg->priv->rx_buf, UART_BUFFER_SIZE );
    }
   
    if( cfg->flags & STM32_UART_ENABLE_TX_IRQ )
    {
      stm32_rbuf_init( &cfg->priv->tx_buf, UART_BUFFER_SIZE );
    }

    NVIC_InitTypeDef NVIC_InitStructure;
    /* NVIC configuration */
    /* Configure the Priority Group to 4 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = cfg->irq_id;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xf;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    if( cfg->flags & STM32_UART_ENABLE_RX_IRQ )
    {
      USART_ITConfig( cfg->periph_base, USART_IT_RXNE, ENABLE);
    }

  }

  /* Enable USART */
  USART_Cmd(cfg->periph_base, ENABLE);

  printf("Initialized USART [0x%08x] irq %d flags 0x%02x\n", cfg->periph_base, cfg->irq_id, cfg->flags );
}

int usart_poll(int id)
{
  return 0;

}

volatile int irqcnt = 0;
static void stm32_uart_generic_irq_handler(struct stm32_uart_cfg *dev)
{
  irqcnt++;

  if (dev->flags & STM32_UART_ENABLE_TX_IRQ)
  {
    // handle TX interrupt
    if (USART_GetITStatus(dev->periph_base, USART_IT_TXE) == SET)
    {
      if (stm32_rbuf_empty(&dev->priv->tx_buf))
      {
        USART_ITConfig(dev->periph_base, USART_IT_TXE, DISABLE);
      }
      else
      {
        uint8_t c = (uint8_t)stm32_rbuf_get(&dev->priv->tx_buf, 1);
        USART_SendData(dev->periph_base, c);
      }
    }
  }
  else
  {
    USART_ITConfig(dev->periph_base, USART_IT_TXE, DISABLE);
  }

  if( dev->flags & STM32_UART_ENABLE_RX_IRQ )
  {
    if (USART_GetFlagStatus(dev->periph_base, USART_FLAG_RXNE) == SET)
    {
        volatile uint8_t ch = USART_ReceiveData(dev->periph_base);
        stm32_rbuf_put( &dev->priv->rx_buf, ch, 1 );
    }

    if (USART_GetFlagStatus(dev->periph_base, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(dev->periph_base, USART_FLAG_ORE);
    }
  }
}

void USART3_IRQHandler()
{
  stm32_uart_generic_irq_handler( &usart_cfg[3] );
}

void USART2_IRQHandler()
{
  stm32_uart_generic_irq_handler( &usart_cfg[2] );
}

void USART1_IRQHandler()
{
  stm32_uart_generic_irq_handler( &usart_cfg[1] );
}

/* non-blocking send using irqs */
void uart_send(int id, uint8_t *buff, int len)
{
    if( id < 0 || id >= UART_MAX_CNT )
      for(;;);

    struct stm32_uart_cfg *cfg = &usart_cfg[id];

    int i;

    if(cfg->flags & STM32_UART_ENABLE_TX_IRQ)
    {
      for( i = 0; i < len; i++ )
          stm32_rbuf_put( &cfg->priv->tx_buf, buff[i], 0 );

      USART_ITConfig( cfg->periph_base, USART_IT_TXE, ENABLE);
    } else {

      //portDISABLE_INTERRUPTS();

      for( i = 0; i < len; i++ )
      {

        while (USART_GetFlagStatus(cfg->periph_base, USART_FLAG_TXE) == RESET)
        {
        }

        USART_SendData(cfg->periph_base, (uint8_t) buff[i]);
      }
    }


}

/* non-blocking send using irqs */
int uart_recv(int id, uint8_t *buff, int len)
{
   struct stm32_uart_cfg *cfg = &usart_cfg[id];

    
    if( cfg->priv->rx_buf.count < len )
      return 0;

    int i;

    for(i=0;i<len;i++)
      buff[i] = stm32_rbuf_get( &cfg->priv->rx_buf, 0 );

    return len;
}

