#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sht31.h" 
#include "dummy_ai.h" 
#include "wifi_manager.h"
#include "firebase_sync.h"
#include "esp_system.h"
#include "actuators.h" 

static const char *TAG = "EDGE_CORE";

sensor_data_t live_data = { .temperature = 28.0, .humidity = 70.0, .pm25 = 15.0 };
ai_decision_t system_decision = { .moss_status = "Loading", .trigger_humidifier = false, .trigger_fan = false };

void sensor_read_task(void *pvParameter) {
    hardware_sensors_init(); 

    while(1) {
        read_hybrid_sensors(&live_data, system_decision.trigger_fan, system_decision.trigger_humidifier);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
    }
}

void edge_ai_task(void *pvParameter) {
    while(1) {
        run_dummy_ai_inference(&live_data, &system_decision);
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    esp_log_level_set("*", ESP_LOG_ERROR);

    esp_log_level_set("EDGE_CORE", ESP_LOG_INFO);
    esp_log_level_set("ACTUATORS", ESP_LOG_INFO);
    esp_log_level_set("EDGE_AI_INFERENCE", ESP_LOG_INFO);
    esp_log_level_set("FIREBASE_SYNC", ESP_LOG_WARN);

    ESP_LOGI("EDGE_CORE", "--- STARTING MOSS AIR PURIFIER EDGE AI ---");

    actuators_init();

    // In ra tổng lượng RAM còn trống của toàn hệ thống trước khi chạy Task
    ESP_LOGW(TAG, "Free Heap Size BEFORE starting tasks: %lu bytes", esp_get_free_heap_size());

    // Tên wifi, mật khẩu wifi
    wifi_init_sta("Repeater", "22222222");

    xTaskCreate(&sensor_read_task, "sensor_task", 4096, NULL, 5, NULL);
    xTaskCreate(&edge_ai_task, "ai_task", 4096, NULL, 4, NULL); 
    xTaskCreate(&firebase_sync_task, "firebase_task", 6144, NULL, 3, NULL);
    xTaskCreate(&firebase_receive_task, "firebase_cmd", 6144, NULL, 3, NULL);

    // In ra tổng lượng RAM còn lại sau khi các luồng đã chia nhau bộ nhớ
    ESP_LOGW(TAG, "Free Heap Size AFTER starting tasks: %lu bytes", esp_get_free_heap_size());
}