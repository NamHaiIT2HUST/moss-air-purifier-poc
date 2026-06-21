#include "firebase_sync.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include "sht31.h" 
#include "dummy_ai.h" 
#include "actuators.h"

static const char *TAG = "FIREBASE_SYNC";

#define FIREBASE_DATA_URL "https://moss-poc-e14fe-default-rtdb.firebaseio.com/sensors/current.json"
#define FIREBASE_CMD_URL "https://moss-poc-e14fe-default-rtdb.firebaseio.com/commands.json" 

extern sensor_data_t live_data;
extern ai_decision_t system_decision;

bool app_force_humidifier = false;

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        if (strstr((char *)evt->data, "\"humidifier\":\"ON\"")) {
            app_force_humidifier = true;
            ESP_LOGW(TAG, "RECEIVED COMMAND FROM APP: Force Humidifier ON!");
        } else {
            app_force_humidifier = false;
        }
    }
    return ESP_OK;
}

void firebase_sync_task(void *pvParameter) {
    char post_data[256];
    vTaskDelay(5000 / portTICK_PERIOD_MS); 

    while(1) {
        esp_http_client_config_t pull_config = {
            .url = FIREBASE_CMD_URL,
            .method = HTTP_METHOD_GET,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .event_handler = http_event_handler,
        };
        esp_http_client_handle_t pull_client = esp_http_client_init(&pull_config);
        esp_http_client_perform(pull_client);
        esp_http_client_cleanup(pull_client);

        bool final_humidifier_state = system_decision.trigger_humidifier || app_force_humidifier;

        snprintf(post_data, sizeof(post_data),
                 "{\"temperature\":%.1f,\"humidity\":%.1f,\"pm25\":%.1f,\"fan_status\":\"%s\",\"humidifier_status\":\"%s\",\"moss_status\":\"%s\"}",
                 live_data.temperature, live_data.humidity, live_data.pm25,
                 system_decision.trigger_fan ? "ON" : "OFF",
                 final_humidifier_state ? "ON" : "OFF",
                 system_decision.moss_status);

        esp_http_client_config_t push_config = {
            .url = FIREBASE_DATA_URL,
            .method = HTTP_METHOD_PATCH,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .crt_bundle_attach = esp_crt_bundle_attach,
        };
        esp_http_client_handle_t push_client = esp_http_client_init(&push_config);
        esp_http_client_set_header(push_client, "Content-Type", "application/json");
        esp_http_client_set_post_field(push_client, post_data, strlen(post_data));
        
        esp_err_t err = esp_http_client_perform(push_client);
        if (err != ESP_OK) ESP_LOGE(TAG, "Cloud Push Failed: %s", esp_err_to_name(err));
        
        esp_http_client_cleanup(push_client);

        vTaskDelay(3000 / portTICK_PERIOD_MS); 
    }
}

void firebase_receive_task(void *pvParameters) {
    char local_response_buffer[256] = {0};

    while (1) {
        esp_http_client_config_t config = {
            .url = "https://moss-poc-e14fe-default-rtdb.firebaseio.com/commands.json",
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .crt_bundle_attach = esp_crt_bundle_attach,
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_method(client, HTTP_METHOD_GET);

        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            int len = esp_http_client_read_response(client, local_response_buffer, sizeof(local_response_buffer) - 1);
            if (len > 0) {
                local_response_buffer[len] = '\0';

                if (strstr(local_response_buffer, "\"force_humidifier\":true")) {
                    ESP_LOGW(TAG, "RECEIVED COMMAND FROM APP: Force Humidifier ON!");
                    control_humidifier(true);
                }
                if (strstr(local_response_buffer, "\"force_fan\":true")) {
                    ESP_LOGW(TAG, "RECEIVED COMMAND FROM APP: Force Fan ON!");
                    control_fan(true);
                }
            }
        }
        esp_http_client_cleanup(client);

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}