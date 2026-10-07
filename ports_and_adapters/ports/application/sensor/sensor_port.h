#ifndef SENSOR_PORT_H
#define SENSOR_PORT_H

#include "../../../domain/inc/temperature_domain.h"

typedef enum
{
    SENSOR_OK = 0,
    SENSOR_ERROR_INVALID_PARAMETER,
    SENSOR_ERROR_TIMEOUT,
    SENSOR_ERROR_CHECKSUM
} SensorStatus;

/*
 * Port de aplicación.
 *
 * Define lo que la Application necesita
 * para obtener una medición.
 */
typedef struct
{
    void *context;
    SensorStatus (*read)(void *context, TemperatureData *data);
} Sensor_Port;

#endif /* SENSOR_PORT_H */
