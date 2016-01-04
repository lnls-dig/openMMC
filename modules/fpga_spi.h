#ifndef FPGA_SPI_H_
#define FPGA_SPI_H_

#include "sdr.h"
#include "utils.h"

#define FPGA_UPDATE_RATE        5000    // in ms
#define FPGA_MEM_ADDR_MAX       0xFF

#define WR_COMMAND              0x80
#define RD_COMMAND              0x00

#define NO_DIAG                 0x00
#define FPGA_TEMP_DEVID         0x01
#define FMC1_TEMP_DEVID         0x02
#define FMC2_TEMP_DEVID         0x03
#define DCDC_TEMP_DEVID         0x04
#define RAM_TEMP_DEVID          0x05
#define FMC1_12V_CURR_DEVID     0x06
#define FMC1_P3V3_CURR_DEVID    0x07
#define FMC1_VADJ_CURR_DEVID    0x08
#define FMC2_12V_CURR_DEVID     0x09
#define FMC2_P3V3_CURR_DEVID    0x0A
#define FMC2_VADJ_CURR_DEVID    0x0B
#define FMC1_12V_DEVID          0x0C
#define FMC1_P3V3_DEVID         0x0D
#define FMC1_VADJ_DEVID         0x0E
#define FMC2_12V_DEVID          0x0F
#define FMC2_P3V3_DEVID         0x10
#define FMC2_VADJ_DEVID         0x11


typedef struct {
#ifdef BF_MS_FIRST
    uint8_t dev_id;
    uint32_t measure:24;
#else
    uint32_t measure:24;
    uint8_t dev_id;
#endif
} t_sensor_diag;

typedef struct __attribute__ ((__packed__)) {
#ifdef BF_MS_FIRST
    uint32_t reserved:26,
        fmc2_pg_c2m:1,
        fmc1_pg_c2m:1,
        fmc2_pg_m2c:1,
        fmc1_pg_m2c:1,
        fmc2_prsnt_m2c_n:1,
        fmc1_prsnt_m2c_n:1;
#else
    uint32_t fmc1_prsnt_m2c_n:1,
        fmc2_prsnt_m2c_n:1,
        fmc1_pg_m2c:1,
        fmc2_pg_m2c:1,
        fmc1_pg_c2m:1,
        fmc2_pg_c2m:1,
        reserved:26;
#endif
} t_fmc_diag;

/* AFC diagnostic struct sent to FPGA via SPI */
typedef struct __attribute__ ((__packed__)) {
    uint32_t cardID[4];
    uint32_t slot_id:16,
        ipmi_addr:16;
    uint32_t data_valid;
    t_sensor_diag sensor[NUM_SENSOR];
    t_fmc_diag fmc_slot;
} board_diagnostic;

typedef union {
    board_diagnostic info;
    uint32_t buffer[sizeof(board_diagnostic)/sizeof(uint32_t)];
} t_board_diagnostic;

void vTaskFPGA_COMM( void * Parameters );
void init_fpga_spi( void );

#endif
