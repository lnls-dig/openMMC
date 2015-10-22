#ifndef LPC17xx_SPI_H_
#define LPC17xx_SPI_H_

#include "chip_lpc175x_6x.h"
#include "ssp_17xx_40xx.h"

#define SSP(n)			LPC_SSP##n

#define ssp_init(id)		Chip_SSP_Init(SSP(id))
#define ssp_deinit(id)		Chip_SSP_DeInit(SSP(id))
#define ssp_flush_rx(id)	Chip_SSP_Int_FlushData(SSP(id))
#define ssp_set_bitrate(id, bitrate) Chip_SSP_SetBitRate(SSP(id), bitrate)

void ssp_config( uint8_t id, uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll );
void ssp_pin_config( uint8_t id );
uint32_t ssp_write( uint8_t id, void * buffer, uint32_t buffer_len);
uint32_t ssp_read( uint8_t id, void * buffer, uint32_t buffer_len);
uint32_t ssp_write_read( uint8_t id, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len );

#endif
