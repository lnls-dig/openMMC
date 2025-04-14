#ifndef MAX11609_
#define MAX11609_

/**
 * @brief Rate at which the MAX11609 sensors are read (in ms)
 */
#define MAX11609_UPDATE_PERIOD    200

#define MAX11609_CHANNEL_0        0
#define MAX11609_CHANNEL_1        1
#define MAX11609_CHANNEL_2        2
#define MAX11609_CHANNEL_3        3
#define MAX11609_CHANNEL_4        4
#define MAX11609_CHANNEL_5        5
#define MAX11609_CHANNEL_6        6
#define MAX11609_CHANNEL_7        7

extern TaskHandle_t vTask11609_Handle;

extern const SDR_type_01h_t SDR_MAX11609_12V_HP;

void MAX11609_init( void );

/**
 * @brief Monitoring task for MAX11609 sensor
 *
 * This task unblocks after every #MAX11609_UPDATE_PERIOD ms and updates the read from all the MAX11609 sensors listed in this module's SDR table
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskMAX11609( void* Parameters );

#endif
