idf_component_register(SRCS "firebase_sync.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_http_client esp-tls esp_crt_bundle freertos hardware_drivers json)