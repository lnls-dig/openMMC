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
 * @file lpc17_spi.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief Legacy SPI driver for LPC17xx
 */

#include "port.h"
#include "pin_mapping.h"

static bool spi_polling = false;
static uint8_t frame_size = 8;

/* Assert SSEL pin */
void spi_assertSSEL(void)
{
    gpio_set_pin_state( PIN_PORT(SPI_SSEL), PIN_NUMBER(SPI_SSEL), GPIO_LEVEL_LOW);
}

/* De-Assert SSEL pin */
void spi_deassertSSEL(void)
{
    gpio_set_pin_state( PIN_PORT(SPI_SSEL), PIN_NUMBER(SPI_SSEL), GPIO_LEVEL_HIGH);
}

void spi_config( uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll )
{
    IRQn_Type irq;
    SPI_CONFIG_FORMAT_T cfg = {0};

    spi_polling = poll;
    frame_size = frame_sz;

    spi_deassertSSEL();

    spi_init(LPC_SPI);

    cfg.bits = SPI_CR_BITS(10);
    cfg.clockMode = SPI_CLOCK_CPHA0_CPOL0;
    cfg.dataOrder = SPI_DATA_MSB_FIRST;

    Chip_SPI_SetFormat(LPC_SPI, &cfg);
    spi_set_bitrate(LPC_SPI, bitrate);

    if (!poll) {
        irq = SPI_IRQn;
        NVIC_SetPriority( irq, configMAX_SYSCALL_INTERRUPT_PRIORITY );
        NVIC_EnableIRQ( irq );
        Chip_SPI_Int_Enable(LPC_SPI);
    }

    spi_flush_rx(LPC_SPI);
}

/* Buffer len in bytes */
uint32_t spi_write( uint8_t * buffer, uint32_t buffer_len)
{
    SPI_DATA_SETUP_T data_st = {0};

    if (spi_polling) {
        data_st.pTxData = buffer;
        data_st.length = buffer_len;
        Chip_SPI_RWFrames_Blocking( LPC_SPI, &data_st );
        return buffer_len;
    } else if ((!spi_polling) && (frame_size <= 8)) {
        data_st.pTxData = buffer;
        data_st.length = buffer_len;
        Chip_SPI_Int_RWFrames8Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;

    } else if ((!spi_polling) && (frame_size <= 16)) {
        data_st.pTxData = buffer;
        data_st.length = buffer_len;
        Chip_SPI_Int_RWFrames16Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;
    }
    return 0;
}


/* WARNING: DO NOT USE! Functions not fully implemented and tested yet */
#if 0
uint32_t spi_read( void * buffer, uint32_t buffer_len)
{
    LPC_SPI_T* LPC_SPI;

    if (id == 0) {
        LPC_SPI = LPC_SPI0;
    } else {
        LPC_SPI = LPC_SPI1;
    }

    Chip_SPI_DATA_SETUP_T data_st = {0};

    if (spi_polling) {
        return (Chip_SPI_ReadFrames_Blocking(LPC_SPI, (uint8_t *) buffer, buffer_len));

    } else if ((!spi_polling) && (frame_size == 8)) {
        data_st.rx_data = buffer;
        data_st.length = buffer_len;
        Chip_SPI_Int_RWFrames8Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        /* Bogus return */
        return buffer_len;

    } else if ((!spi_polling) && (frame_size == 16)) {
        data_st.rx_data = buffer;
        data_st.length = buffer_len;
        Chip_SPI_Int_RWFrames16Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;
    }
    return 0;
}

uint32_t spi_write_read( uint8_t id, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len )
{
    LPC_SPI_T* LPC_SPI;

    if (id == 0) {
        LPC_SPI = LPC_SPI0;
    } else {
        LPC_SPI = LPC_SPI1;
    }

    Chip_SPI_DATA_SETUP_T data_st = {0};

    if (spi_polling) {
        data_st.tx_data = tx_buf;
        data_st.rx_data = rx_buf;
        data_st.length = tx_len+rx_len;
        return (Chip_SPI_RWFrames_Blocking(LPC_SPI, &data_st));
    } else if ((!spi_polling) && (frame_size == 8)) {
        data_st.tx_data = tx_buf;
        data_st.length = tx_len;
        Chip_SPI_Int_RWFrames8Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        /* Bogus return */
        return rx_len;

    } else if ((!spi_polling) && (frame_size == 16)) {
        data_st.tx_data = tx_buf;
        data_st.length = tx_len;
        Chip_SPI_Int_RWFrames16Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return rx_len;
    }
    return 0;

}
#endif
