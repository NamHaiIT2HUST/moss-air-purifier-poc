#ifndef SHT31_H
#define SHT31_H

#include "esp_err.h"
#include <stdbool.h>

typedef struct {
    float temperature;
    float humidity;
    float pm25;
} sensor_data_t;

esp_err_t hardware_sensors_init(void);
void read_hybrid_sensors(sensor_data_t *data, bool is_fan_on, bool is_humidifier_on);

#endif 