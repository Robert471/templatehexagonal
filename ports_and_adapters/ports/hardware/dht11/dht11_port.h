#ifndef DHT11_PORT_H
#define DHT11_PORT_H

#include <stdint.h>

typedef enum
{
    DHT11_PIN_INPUT = 0,
    DHT11_PIN_OUTPUT
} Dht11_PinMode;

typedef enum
{
    DHT11_PIN_LOW = 0,
    DHT11_PIN_HIGH
} Dht11_PinState;

/*
 * Contrato de hardware requerido por el DHT11 Adapter.
 *
 * El port no conoce STM32 ni HAL.
 */
typedef struct
{
    /* Contexto de GPIO: utilizado por set_mode/write/read. */
    void *gpio_context;

    /* Contexto de temporización: utilizado exclusivamente por delay_us. */
    void *timer_context;

    void (*set_mode)(void *context, Dht11_PinMode mode);
    void (*write)(void *context, Dht11_PinState state);
    Dht11_PinState (*read)(void *context);
    void (*delay_us)(void *context, uint32_t microseconds);
} Dht11_Port;

#endif /* DHT11_PORT_H */
