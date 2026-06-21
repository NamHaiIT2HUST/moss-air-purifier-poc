#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAN_PIN GPIO_NUM_4
#define HUMIDIFIER_PIN GPIO_NUM_5

void actuators_init(void);

void control_fan(bool state);
void control_humidifier(bool state);

#ifdef __cplusplus
}
#endif

#endif // ACTUATORS_H