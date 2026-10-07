#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "stm32f4xx_hal.h"
#include "../../../../ports/hardware/dht11/dht11_port.h"

/*
 * Driver STM32: encapsula únicamente detalles del HAL.
 *
 * El contexto contiene el puerto y pin concretos. Esto evita variables
 * globales ocultas y permite reutilizar el mismo driver con otra GPIO.
 */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} Gpio_Driver;

void gpio_driver_init(
    Gpio_Driver *driver,
    GPIO_TypeDef *port,
    uint16_t pin
);

void gpio_driver_set_mode(
    void *context,
    Dht11_PinMode mode
);

void gpio_driver_write(
    void *context,
    Dht11_PinState state
);

Dht11_PinState gpio_driver_read(
    void *context
);

#endif /* GPIO_DRIVER_H */
