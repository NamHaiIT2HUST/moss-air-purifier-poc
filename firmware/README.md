# 🌿 Moss Air Purifier - EdgeAI Firmware (PoC)

Đây là mã nguồn điều khiển thiết bị tại biên (Edge Device) cho dự án Máy lọc không khí bằng rêu sinh học. Hệ thống được phát triển trên nền tảng **ESP-IDF** (C/C++) kết hợp **FreeRTOS** để tối ưu hóa năng lực xử lý đa nhiệm, nhắm tới mục tiêu Edge Computing thực thụ.

## 🏗️ Kiến trúc Phần mềm (Component-Based)

Dự án được chia module hóa để đảm bảo tính mở rộng và khả năng chịu lỗi (Fault Tolerance):

* **`hardware_drivers/`**: Module giao tiếp ngoại vi (I2C/UART).
    * Hiện tại đang sử dụng cấu trúc **Hybrid Driver**: Tự động nhận diện cảm biến SHT31/PMS7003 vật lý. Nếu không có phần cứng, hệ thống tự động fallback sang cơ chế Mock Data (chạy kịch bản toán học mô phỏng hệ sinh thái) giúp việc dev/test phần mềm không bị gián đoạn.
* **`edge_ai_core/`**: Bộ não của hệ thống.
    * Hiện tại đang chạy **Rule-based Dummy AI** dựa trên tham số sinh học chuẩn của rêu *Hypnum cupressiforme* (Độ ẩm sống: 60% - 90%). 
    * Đã thiết kế sẵn cấu trúc `ai_decision_t` chờ tích hợp model TinyML (.tflite) ở các bản cập nhật sau.
* **`main/`**: Đóng vai trò nhạc trưởng (Orchestrator). Sử dụng FreeRTOS phân luồng và quản lý bộ nhớ dùng chung (Shared Memory) giữa cảm biến và AI.

## ⚙️ Cấu trúc Đa nhiệm (FreeRTOS Tasks)

1.  **`sensor_read_task` (Priority 5):** Chạy chu kỳ 2000ms. Quét dữ liệu môi trường (Nhiệt độ, Độ ẩm, PM2.5) và phản ứng với các lệnh điều khiển (bật quạt, phun sương) từ luồng AI.
2.  **`edge_ai_task` (Priority 4):** Chạy chu kỳ 3000ms. Suy luận dữ liệu tại biên, gán nhãn trạng thái rêu (`Healthy`, `Dehydrated`) và ra quyết định can thiệp phần cứng hoàn toàn độc lập mà không cần kết nối Cloud.
3.  **`network_sync_task` (Pending):** Luồng xử lý giao thức Wi-Fi và đồng bộ dữ liệu Realtime Database (Firebase).

## 🚀 Tiến độ (Roadmap)

- [x] Phase 1: Hardware Integration & Hybrid Mock Driver.
- [x] Phase 2: Edge Inference & FreeRTOS Task Routing.
- [ ] Phase 3: Data Pipeline (Wi-Fi Manager & Firebase Sync).
- [ ] Phase 4: TinyML Model Integration (Replace Dummy AI).

## 🛠️ Hướng dẫn Biên dịch (Build)

Cần cài đặt môi trường ESP-IDF (v4.4 hoặc v5.x) trước khi biên dịch:

```bash
idf.py set-target esp32
idf.py build
idf.py -p (PORT) flash monitor