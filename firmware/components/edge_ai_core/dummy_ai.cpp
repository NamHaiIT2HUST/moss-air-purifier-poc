#include "dummy_ai.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "EDGE_AI_INFERENCE";

extern "C" void run_dummy_ai_inference(sensor_data_t *current_data, ai_decision_t *decision) {
    ESP_LOGI(TAG, "Running Inference on Edge...");

    if (current_data->humidity < 60.0) {
        strcpy(decision->moss_status, "Dehydrated");
        decision->trigger_humidifier = true;
        ESP_LOGW(TAG, "ALERT: Moss Dehydrated! Triggering Humidifier.");
    } else if (current_data->humidity > 85.0) {
        strcpy(decision->moss_status, "Over-hydrated");
        decision->trigger_humidifier = false;
        ESP_LOGW(TAG, "ALERT: Moss too wet. Stopping Humidifier.");
    } else {
        strcpy(decision->moss_status, "Healthy");
        decision->trigger_humidifier = false;
    }

    if (current_data->pm25 > 35.0) {
        decision->trigger_fan = true;
        ESP_LOGW(TAG, "ALERT: PM2.5 High! Triggering Air Fan.");
    } else if (current_data->pm25 < 15.0) {
        decision->trigger_fan = false;
    }

    ESP_LOGI(TAG, "Inference Result -> Status: %s | Humidifier: %d | Fan: %d", 
             decision->moss_status, decision->trigger_humidifier, decision->trigger_fan);
}