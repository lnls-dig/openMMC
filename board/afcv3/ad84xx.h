#ifndef AD84XX_H_
#define AD84XX_H_

#define DAC_VADJ_SPI_SPEED    10000000
#define DAC_VADJ_FRAME_SIZE   8

void dac_vadj_init( void );
void dac_vadj_config( uint8_t addr, uint8_t val );

#endif
