#include "port.h"
#include "pin_mapping.h"


static bool ssp_polling = false;
static uint8_t frame_size = 8;

void ssp_config( uint8_t id, uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll )
{
    IRQn_Type irq;
    LPC_SSP_T* ssp_id;

    if (id == 0) {
        ssp_id = LPC_SSP0;
    } else {
        ssp_id = LPC_SSP1;
    }

    ssp_polling = poll;
    frame_size = frame_sz;

    ssp_pin_config(id);
    Chip_SSP_Init(ssp_id);
    Chip_SSP_SetBitRate(ssp_id, bitrate);
    Chip_SSP_SetMaster(ssp_id, master_mode);
    Chip_SSP_SetFormat(ssp_id, (frame_sz-1), SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);

    if (!poll) {
        switch (id) {
        case 0:
            irq = SSP0_IRQn;
            break;
        case 1:
            irq = SSP1_IRQn;
            break;
        }
        NVIC_SetPriority( irq, configMAX_SYSCALL_INTERRUPT_PRIORITY );
        NVIC_EnableIRQ( irq );
    }
}

void ssp_pin_config( uint8_t id )
{
    /* Set up clock and muxing for SSP0/1 interface */
    switch( id ) {
    case FPGA_SPI:
        Chip_IOCON_PinMux(LPC_IOCON, 1, 20, IOCON_MODE_PULLDOWN, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 1, 21, IOCON_MODE_PULLUP, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 1, 23, IOCON_MODE_INACT, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 1, 24, IOCON_MODE_INACT, IOCON_FUNC3);
        break;

    case FLASH_SPI:
        Chip_IOCON_PinMux(LPC_IOCON, 0, 6, IOCON_MODE_PULLDOWN, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_PULLUP, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 8, IOCON_MODE_INACT, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC3);
        break;

    case DAC_VADJ_SPI:
	Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_PULLDOWN, IOCON_FUNC3);
        Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_PULLUP, IOCON_FUNC3);
	/* Pin 17 is used for PROGRAM_B, as we don't receive any data, the MISO pin is not used */
        Chip_IOCON_PinMux(LPC_IOCON, 0, 18, IOCON_MODE_INACT, IOCON_FUNC3);
	break;
    }
}

/* Buffer len in bytes */
uint32_t ssp_write( uint8_t id, void * buffer, uint32_t buffer_len)
{
    LPC_SSP_T* ssp_id;

    switch( id ) {
    case FPGA_SPI:
    case DAC_VADJ_SPI:
	ssp_id = LPC_SSP0;
	break;
    case FLASH_SPI:
        ssp_id = LPC_SSP1;
	break;
    }

    Chip_SSP_DATA_SETUP_T data_st = {0};

    if (ssp_polling) {
        return (Chip_SSP_WriteFrames_Blocking(ssp_id, (uint8_t *) buffer, buffer_len));
    } else if ((!ssp_polling) && (frame_size <= 8)) {
	data_st.tx_data = (uint8_t *)buffer;
        data_st.length = buffer_len;
        Chip_SSP_Int_RWFrames8Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;

    } else if ((!ssp_polling) && (frame_size <= 16)) {
        data_st.tx_data = (uint16_t *)buffer;
        data_st.length = buffer_len;
        Chip_SSP_Int_RWFrames16Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;
    }
    return 0;
}

uint32_t ssp_read( uint8_t id, void * buffer, uint32_t buffer_len)
{
    LPC_SSP_T* ssp_id;

    if (id == 0) {
        ssp_id = LPC_SSP0;
    } else {
        ssp_id = LPC_SSP1;
    }

    Chip_SSP_DATA_SETUP_T data_st = {0};

    if (ssp_polling) {
        return (Chip_SSP_ReadFrames_Blocking(ssp_id, (uint8_t *) buffer, buffer_len));

    } else if ((!ssp_polling) && (frame_size == 8)) {
        data_st.tx_data = buffer;
        data_st.length = buffer_len;
        Chip_SSP_Int_RWFrames8Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        /* Bogus return */
        return buffer_len;

    } else if ((!ssp_polling) && (frame_size == 16)) {
        data_st.tx_data = buffer;
        data_st.length = buffer_len;
        Chip_SSP_Int_RWFrames16Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return buffer_len;
    }
    return 0;
}

uint32_t ssp_write_read( uint8_t id, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len )
{
    LPC_SSP_T* ssp_id;

    if (id == 0) {
        ssp_id = LPC_SSP0;
    } else {
        ssp_id = LPC_SSP1;
    }

    Chip_SSP_DATA_SETUP_T data_st = {0};

    if (ssp_polling) {
	data_st.tx_data = tx_buf;
	data_st.rx_data = rx_buf;
	data_st.length = tx_len+rx_len;
        return (Chip_SSP_RWFrames_Blocking(ssp_id, &data_st));

    } else if ((!ssp_polling) && (frame_size == 8)) {
        data_st.tx_data = tx_buf;
        data_st.length = tx_len;
        Chip_SSP_Int_RWFrames8Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        /* Bogus return */
        return rx_len;

    } else if ((!ssp_polling) && (frame_size == 16)) {
        data_st.tx_data = tx_buf;
        data_st.length = tx_len;
        Chip_SSP_Int_RWFrames16Bits( ssp_id, &data_st );
        /* BUG: We're not verifying if the message was trasmitted */
        return rx_len;
    }
    return 0;

}
