#include "dummy_ai.h"
#include "esp_log.h"
#include <string.h>
#include "actuators.h"

static const char *TAG = "EDGE_AI_INFERENCE";

// ==============================================================
// KHUNG KHỞI TẠO TENSORFLOW LITE MICRO (Chờ file model)
// ==============================================================
/*
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

// File mảng byte của model
// #include "moss_model_data.h" 

const tflite::Model* g_model = nullptr;
tflite::MicroInterpreter* g_interpreter = nullptr;
TfLiteTensor* g_input = nullptr;
TfLiteTensor* g_output = nullptr;

// Cấp phát 15KB RAM tĩnh cho AI
constexpr int kTensorArenaSize = 15 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
*/

// Bọc extern "C" để file main.c (thuần C) có thể gọi được lúc khởi động
extern "C" void edge_ai_init() {
    ESP_LOGI(TAG, "Initializing Edge AI Hardware & Memory...");
    
    /*
    g_model = tflite::GetModel(g_moss_model_data);
    
    // Đăng ký các phép toán
    static tflite::MicroMutableOpResolver<3> resolver;
    resolver.AddFullyConnected();
    resolver.AddRelu();
    resolver.AddSoftmax();

    // Khởi tạo bộ thông dịch AI
    static tflite::MicroInterpreter static_interpreter(
        g_model, resolver, tensor_arena, kTensorArenaSize);
    g_interpreter = &static_interpreter;
    
    // Phân bổ bộ nhớ
    g_interpreter->AllocateTensors();
    
    // Gắn trỏ vào ngõ vào/ngõ ra
    g_input = g_interpreter->input(0);
    g_output = g_interpreter->output(0);
    
    ESP_LOGI(TAG, "TensorFlow Lite Micro loaded successfully!");
    */
}

// Luồng Dummy hiện tại - Sẽ được thay thế bằng g_interpreter->Invoke() khi có model
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