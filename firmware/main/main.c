#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "EDGE_AI_CORE";

void sensor_read_task(void *pvParameter) {
    while(1) {
        ESP_LOGI(TAG, "Reading Hardware Sensors (Temp, Humidity, PM2.5)...");

        vTaskDelay(2000 / portTICK_PERIOD_MS); 
    }
}

void edge_ai_task(void *pvParameter) {
    while(1) {
        ESP_LOGI(TAG, "Running Edge AI Inference: Checking Moss Health...");

        vTaskDelay(5000 / portTICK_PERIOD_MS); 
    }
}

void network_sync_task(void *pvParameter) {
    while(1) {
        ESP_LOGW(TAG, "Routing Data -> Syncing to Firebase Realtime DB...");
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "--- MOSS AIR PURIFIER EDGE DEVICE BOOTING ---");

    xTaskCreate(&sensor_read_task, "sensor_task", 2048, NULL, 5, NULL);
    xTaskCreate(&network_sync_task, "network_task", 4096, NULL, 4, NULL);
    xTaskCreate(&edge_ai_task, "ai_inference_task", 8192, NULL, 3, NULL); 
}