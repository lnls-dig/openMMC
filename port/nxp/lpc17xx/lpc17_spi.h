#ifndef LPC17xx_SPI_H_
#define LPC17xx_SPI_H_

#include "spi_17xx_40xx.h"

#define spi_init(id)			Chip_SPI_Init(id)
#define spi_deinit(id)			Chip_SPI_DeInit(id)
#define spi_flush_rx(id)		Chip_SPI_Int_FlushData(id)
#define spi_set_bitrate(id, bitrate)	Chip_SPI_SetBitRate(id, bitrate)

#define SPI_SLAVE			0
#define SPI_MASTER			1
#define SPI_INTERRUPT			0
#define SPI_POLLING			1

void spi_config( uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll );
uint32_t spi_write( void * buffer, uint32_t buffer_len );

#endif
