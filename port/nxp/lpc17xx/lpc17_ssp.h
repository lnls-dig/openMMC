#ifndef LPC17xx_SSP_H_
#define LPC17xx_SSP_H_

#include "chip_lpc175x_6x.h"
#include "ssp_17xx_40xx.h"
#include "task.h"

#define MAX_SSP_INTERFACES      3
#define SSP(n)                  LPC_SSP##n

#define SSP_SLAVE        0
#define SSP_MASTER       1
#define SSP_INTERRUPT    0
#define SSP_POLLING      1

typedef enum {
    ASSERT = 0,
    DEASSERT
} t_ssel_state;

typedef struct {
    uint8_t port;
    uint8_t sck_pin;
    uint8_t sck_mode;
    uint8_t sck_func;
    uint8_t mosi_pin;
    uint8_t mosi_mode;
    uint8_t mosi_func;
    uint8_t miso_pin;
    uint8_t miso_mode;
    uint8_t miso_func;
    uint8_t ssel_pin;
    uint8_t ssel_mode;
    uint8_t ssel_func;
} t_ssp_pin;

typedef struct {
    LPC_SSP_T * lpc_id;
    IRQn_Type irq;
    uint32_t bitrate;
    uint8_t master_mode;
    uint8_t polling;
    uint8_t frame_size;
    const t_ssp_pin * pin_cfg;
    Chip_SSP_DATA_SETUP_T xf_setup;
    TaskHandle_t caller_task;
} t_ssp_config;

void ssp_init( uint8_t id, uint32_t bitrate, uint8_t frame_sz, bool master_mode, bool poll );
void ssp_ssel_control( uint8_t id, t_ssel_state state );
void ssp_write_read( uint8_t id, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len );

#define ssp_chip_init(id)                   Chip_SSP_Init(SSP(id))
#define ssp_chip_deinit(id)                 Chip_SSP_DeInit(SSP(id))
#define ssp_flush_rx(id)                    Chip_SSP_Int_FlushData(SSP(id))
#define ssp_set_bitrate(id, bitrate)        Chip_SSP_SetBitRate(SSP(id), bitrate)
#define ssp_write(id, buffer, buffer_len)   ssp_write_read(id, buffer, buffer_len, NULL, 0)
#define ssp_read(id, buffer, buffer_len)    ssp_write_read(id, NULL, 0, buffer, buffer_len)


#endif
