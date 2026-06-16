import time
import random
import requests
import json

FIREBASE_URL = "https://moss-poc-e14fe-default-rtdb.firebaseio.com/sensors.json"

# Khởi tạo giá trị môi trường ban đầu
current_temp = 28.5
current_hum = 58.0
current_pm25 = 45.0
current_pm10 = 55.0
current_light = 300

def generate_mock_data():
    global current_temp, current_hum, current_pm25, current_pm10, current_light

    # 1. Đọc trạng thái thiết bị hiện tại
    fan_on = current_pm25 > 25.0
    humidifier_on = current_hum < 60.0

    # 2. Logic phản hồi môi trường (Adaptive Control Simulation)
    if fan_on:
        # Máy lọc đang chạy -> Bụi giảm nhanh
        current_pm25 += random.uniform(-2.5, -0.5)
        current_pm10 += random.uniform(-2.5, -0.5)
    else:
        # Máy lọc tắt -> Bụi từ từ tăng lên (do rò rỉ từ ngoài vào)
        current_pm25 += random.uniform(0.1, 1.0)
        current_pm10 += random.uniform(0.1, 1.0)

    if humidifier_on:
        # Phun sương đang bật -> Độ ẩm tăng
        current_hum += random.uniform(0.5, 2.0)
    else:
        # Phun sương tắt -> Rêu hút nước, môi trường khô dần -> Độ ẩm giảm
        current_hum += random.uniform(-0.8, -0.1)

    # Nhiệt độ và ánh sáng dao động tự nhiên theo môi trường
    current_temp += random.uniform(-0.2, 0.2)
    current_light += random.randint(-15, 15)

    # 3. Khóa giới hạn (Clamp) để số liệu luôn hợp lý
    current_temp = max(15.0, min(current_temp, 40.0))
    current_hum = max(30.0, min(current_hum, 90.0))
    current_pm25 = max(5.0, current_pm25)
    current_pm10 = max(5.0, current_pm10)
    current_light = max(0, current_light)

    return {
        "temperature": round(current_temp, 1),
        "humidity": round(current_hum, 1),
        "pm25": round(current_pm25, 1),
        "pm10": round(current_pm10, 1),
        "light_intensity": int(current_light),
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "fan_status": "ON" if fan_on else "OFF",
        "humidifier_status": "ON" if humidifier_on else "OFF",
        # Placeholder cho module Edge AI của Đồng Đồng cập nhật vào sau
        "moss_status": "Healthy" 
    }

print("🚀 Khởi động Hệ thống Giả lập IoT (Simulation Engine)...")
print(f"📡 Đang kết nối tới: {FIREBASE_URL}\n")

CURRENT_URL = "https://moss-poc-e14fe-default-rtdb.firebaseio.com/sensors/current.json"
HISTORY_URL = "https://moss-poc-e14fe-default-rtdb.firebaseio.com/sensors/history.json"

while True:
    data = generate_mock_data()
    try:
        res_current = requests.patch(CURRENT_URL, data=json.dumps(data))

        res_history = requests.post(HISTORY_URL, data=json.dumps(data))
        
        if res_current.status_code == 200 and res_history.status_code == 200:
            print(f"✅ Đã lưu Realtime & History: Temp {data['temperature']}°C | Hum {data['humidity']}% | PM2.5 {data['pm25']}")
        else:
            print(f"❌ Lỗi Firebase: HTTP {res_current.status_code}")
            
    except requests.exceptions.RequestException as e:
        print(f"⚠️ Lỗi kết nối mạng: {e}")

    time.sleep(3)