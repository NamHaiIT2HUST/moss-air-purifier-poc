#ifndef DUMMY_AI_H
#define DUMMY_AI_H

#include "sht31.h" 

typedef struct {
    char moss_status[20];
    bool trigger_humidifier;
    bool trigger_fan;
} ai_decision_t;

void run_dummy_ai_inference(sensor_data_t *current_data, ai_decision_t *decision);

#endif