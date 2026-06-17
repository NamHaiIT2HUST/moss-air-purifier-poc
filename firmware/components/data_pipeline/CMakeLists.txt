idf_component_register(SRCS "wifi_manager.c" "firebase_sync.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_wifi nvs_flash esp_event esp_netif esp_http_client mbedtls hardware_drivers edge_ai_core)