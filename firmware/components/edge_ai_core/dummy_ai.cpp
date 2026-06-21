#include "dummy_ai.h"
#include "esp_log.h"
#include <string.h>
#include "actuators.h"

// ==============================================================
// --- KHUNG KIẾN TRÚC TENSORFLOW LITE MICRO (SẴN SÀNG NHÚNG) ---
// ==============================================================
/*
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

// 1. Chứa file mảng byte của Model thật
// #include "moss_model_data.h" 

// 2. Cấp phát vùng nhớ RAM tĩnh (Arena) cho Neural Network (VD: 15KB)
constexpr int kTensorArenaSize = 15 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// 3. Khai báo các con trỏ toàn cục của TFLite
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
*/
// ==============================================================

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

    // Gọi lệnh điều khiển phần cứng thực tế
    control_fan(decision->trigger_fan);
    control_humidifier(decision->trigger_humidifier);
}