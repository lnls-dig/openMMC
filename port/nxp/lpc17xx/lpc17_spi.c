#include "port.h"
#include "pin_mapping.h"

static bool spi_polling = false;
static uint8_t frame_size = 8;

/* Assert SSEL pin */
void spi_assertSSEL(void)
{
    Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, false);
}

/* De-Assert SSEL pin */
void spi_deassertSSEL(void)
{
    Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, true);
}

void spi_config( uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll )
{
    IRQn_Type irq;
    SPI_CONFIG_FORMAT_T cfg = {0};

    spi_polling = poll;
    frame_size = frame_sz;

    /* Set up clock and muxing for SPI interface */
    Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_PULLDOWN, IOCON_FUNC3);
    Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_PULLUP, IOCON_FUNC0);
    /* Pin 17 is used for PROGRAM_B, as we don't receive any data, the MISO pin is not used */
    Chip_IOCON_PinMux(LPC_IOCON, 0, 18, IOCON_MODE_INACT, IOCON_FUNC3);
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
uint32_t spi_write( void * buffer, uint32_t buffer_len)
{
    SPI_DATA_SETUP_T data_st = {0};

    if (spi_polling) {
	data_st.pTxData = buffer;
	data_st.length = buffer_len;
	Chip_SPI_RWFrames_Blocking( LPC_SPI, &data_st );
	return buffer_len;
    } else if ((!spi_polling) && (frame_size <= 8)) {
	data_st.pTxData = (uint8_t *)buffer;
        data_st.length = buffer_len;
	Chip_SPI_Int_RWFrames8Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;

    } else if ((!spi_polling) && (frame_size <= 16)) {
        data_st.pTxData = (uint8_t *)buffer;
        data_st.length = buffer_len;
        Chip_SPI_Int_RWFrames16Bits( LPC_SPI, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;
    }
    return 0;
}

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
