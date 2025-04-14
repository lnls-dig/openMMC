/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "port.h"

/* Project Includes */
#include "sdr.h"
#include "task_priorities.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "max11609.h"
#include "utils.h"
#include "uart_debug.h"
#include "max116xx.h"

TaskHandle_t vTask11609_Handle;

void vTaskMAX11609( void* Parameters )
{
    max116xx_cfg max11609_cfg = {
        .ref_sel   = MAX116XX_REF_INT_ON_OUT,
        .clk_sel   = MAX116XX_CLK_INT,
        .pol_sel   = MAX116XX_UNIPOLAR,
        .scan_mode = MAX116XX_SCAN_OFF_SINGLE_CONV,
        .diff_mode = MAX116XX_SINGLE_ENDED,
        .channel_sel = 0
    };
    const TickType_t update_period = pdMS_TO_TICKS(MAX11609_UPDATE_PERIOD);

    sensor_t * voltage_sensor;
    int16_t data_voltage[1];
    mmc_err err;

    for ( ;; ) {
        /* Iterate through the SDR Table to find all the MAX11609 entries */
        for ( voltage_sensor = sdr_head; voltage_sensor != NULL; voltage_sensor = voltage_sensor->next) {
            if ( voltage_sensor->task_handle == NULL ) {
                continue;
            }
            /* Check if this task should update the selected SDR */
            if ( *(voltage_sensor->task_handle) != xTaskGetCurrentTaskHandle() ) {
                continue;
            }

            max11609_cfg.channel_sel = ((SDR_type_01h_t*)voltage_sensor->sdr)->OEM;
            err = max116xx_set_config(voltage_sensor->chipid, &max11609_cfg);
            if(err != MMC_OK) continue;

            err = max116xx_read_uni(voltage_sensor->chipid, data_voltage, 1);
            if (err == MMC_OK)
            {
                voltage_sensor->readout_value = (uint16_t)(data_voltage[0] >> 2);
            }

            sensor_state_check(voltage_sensor);
            check_sensor_event(voltage_sensor);
        }
        vTaskDelay(update_period);
    }
}

void MAX11609_init()
{
    xTaskCreate(vTaskMAX11609, "MAX11609", 256, (void *) NULL, tskMAX11609SENSOR_PRIORITY, &vTask11609_Handle);
}
