#ifndef COMM_PORT_H
#define COMM_PORT_H

#include "../../../domain/inc/temperature_domain.h"

typedef enum
{
    COMM_OK = 0,
    COMM_ERROR_INVALID_PARAMETER,
    COMM_ERROR
} CommStatus;

/*
 * Port de aplicación.
 *
 * Define lo que Application necesita
 * para transmitir una medición.
 */
typedef struct
{
    void *context;
    CommStatus (*send_temperature)(void *context, const TemperatureData *data);
} Comm_Port;

#endif /* COMM_PORT_H */
