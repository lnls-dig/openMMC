#ifndef SDR_LIST_H_
#define SDR_LIST_H_

#include "sdr.h"

#define SDR_ARRAY_LENGTH (sizeof(sensor_array) / sizeof(sensor_array[0]))

sensor_data_entry_t sdrData[NUM_SDR];
const sensor_t const sensor_array[NUM_SDR];


#endif
