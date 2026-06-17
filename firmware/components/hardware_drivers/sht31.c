#include "sht31.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdlib.h>

static const char *TAG = "HW_DRIVER";

#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define SHT31_SENSOR_ADDR           0x44

esp_err_t hardware_sensors_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    esp_err_t ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (ret == ESP_OK) ESP_LOGI(TAG, "I2C Master initialized on SDA:21, SCL:22");
    return ret;
}

void read_hybrid_sensors(sensor_data_t *data, bool is_fan_on, bool is_humidifier_on) {
    uint8_t sht31_cmd[2] = {0x24, 0x00};
    uint8_t data_rd[6];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT31_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, sht31_cmd, 2, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        // ĐỌC CẢM BIẾN THẬT
        vTaskDelay(50 / portTICK_PERIOD_MS);
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (SHT31_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
        i2c_master_read(cmd, data_rd, sizeof(data_rd), I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        uint16_t t_ticks = (data_rd[0] << 8) | data_rd[1];
        uint16_t rh_ticks = (data_rd[3] << 8) | data_rd[4];
        data->temperature = -45 + (175 * (float)t_ticks / 65535);
        data->humidity = 100 * (float)rh_ticks / 65535;
        ESP_LOGI(TAG, "[REAL SENSOR] T: %.1f C, H: %.1f %%", data->temperature, data->humidity);
    } else {
        // MOCK DATA
        if (is_humidifier_on) {
            data->humidity = (data->humidity + 4.0 > 90.0) ? 90.0 : data->humidity + 4.0;
            data->temperature = (data->temperature - 0.2 < 24.0) ? 24.0 : data->temperature - 0.2;
        } else {
            data->humidity = (data->humidity - 0.5 < 50.0) ? 50.0 : data->humidity - 0.5;
        }

        if (is_fan_on) {
            data->pm25 = (data->pm25 * 0.85 < 5.0) ? 5.0 : data->pm25 * 0.85;
        } else {
            data->pm25 = (data->pm25 + 1.5 > 45.0) ? 45.0 : data->pm25 + 1.5;
        }

        // Add Jitter
        data->humidity += (((rand() % 100) / 100.0) - 0.5);
        data->pm25 += (((rand() % 200) / 100.0) - 1.0);

        ESP_LOGW(TAG, "[MOCK SENSOR] T: %.1f C, H: %.1f %%, PM2.5: %.1f", data->temperature, data->humidity, data->pm25);
    }
}