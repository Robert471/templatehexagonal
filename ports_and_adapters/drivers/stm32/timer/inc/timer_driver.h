#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* Driver de temporización inyectable; no depende de un TIM global. */
typedef struct
{
    TIM_HandleTypeDef *handle;
} Timer_Driver;

void timer_driver_init(
    Timer_Driver *driver,
    TIM_HandleTypeDef *handle
);

void timer_driver_delay_us(
    void *context,
    uint32_t us
);

#endif /* TIMER_DRIVER_H */
