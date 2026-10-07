#ifndef SENSOR_COMPOSITION_H
#define SENSOR_COMPOSITION_H

#include "stm32f4xx_hal.h"
#include "../../../application/inc/sensor_application.h"

/* Ensambla el sensor real. La aplicación no conoce esta función interna. */
void sensor_composition_init(
    Sensor_Application *application,
    TIM_HandleTypeDef *timer_handle
);

#endif /* SENSOR_COMPOSITION_H */
