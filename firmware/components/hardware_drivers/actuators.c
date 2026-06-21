#include "actuators.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ACTUATORS";

void actuators_init(void) {
    ESP_LOGI(TAG, "Initializing GPIO for Actuators...");

    gpio_reset_pin(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(FAN_PIN, 0); 

    gpio_reset_pin(HUMIDIFIER_PIN);
    gpio_set_direction(HUMIDIFIER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(HUMIDIFIER_PIN, 0); 
}

void control_fan(bool state) {
    gpio_set_level(FAN_PIN, state ? 1 : 0);
    ESP_LOGI(TAG, "Air Fan is now %s", state ? "ON" : "OFF");
}

void control_humidifier(bool state) {
    gpio_set_level(HUMIDIFIER_PIN, state ? 1 : 0);
    ESP_LOGI(TAG, "Humidifier is now %s", state ? "ON" : "OFF");
}