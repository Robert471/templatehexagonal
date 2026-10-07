#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stdint.h>

/*
 * Errores propios del puerto de hardware.
 * No reutilizamos CommStatus porque ese tipo pertenece a Application.
 * Esto mantiene la dirección de dependencias de la arquitectura hexagonal.
 */
typedef enum
{
    SERIAL_OK = 0,
    SERIAL_ERROR_INVALID_PARAMETER,
    SERIAL_ERROR_TRANSMIT
} SerialStatus;

typedef struct
{
    void *context;

    SerialStatus (*send)(
        void *context,
        const uint8_t *data,
        uint16_t length
    );

} Serial_Port;

#endif /* SERIAL_PORT_H */
