#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sht31.h" 

static const char *TAG = "EDGE_CORE";

sensor_data_t live_data = { .temperature = 28.0, .humidity = 70.0, .pm25 = 15.0 };
bool fan_active = false;
bool humidifier_active = false;

void sensor_read_task(void *pvParameter) {
    hardware_sensors_init(); // Khởi tạo I2C từ Component

    while(1) {
        read_hybrid_sensors(&live_data, fan_active, humidifier_active);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "--- STARTING MOSS AIR PURIFIER EDGE AI ---");

    xTaskCreate(&sensor_read_task, "sensor_task", 4096, NULL, 5, NULL);
    
    // Tương lai: xTaskCreate cho AI và Firebase sẽ đặt ở đây
}