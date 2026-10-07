#ifndef USART_DRIVER_H
#define USART_DRIVER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "../../../../ports/hardware/serial/serial_port.h"

/* Driver UART desacoplado de una instancia global de HAL. */
typedef struct
{
    UART_HandleTypeDef *handle;
    uint32_t timeout;
} Usart_Driver;

void usart_driver_init(
    Usart_Driver *driver,
    UART_HandleTypeDef *handle,
    uint32_t timeout
);

SerialStatus usart_driver_send(
    void *context,
    const uint8_t *data,
    uint16_t size
);

#endif /* USART_DRIVER_H */
