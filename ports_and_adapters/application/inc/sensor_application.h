#ifndef SENSOR_APPLICATION_H
#define SENSOR_APPLICATION_H

#include "../../domain/inc/temperature_domain.h"
#include "../../ports/application/sensor/sensor_port.h"


typedef struct
{
    Sensor_Port sensor_port;

} Sensor_Application;


void sensor_application_init(
    Sensor_Application *application,
    const Sensor_Port *sensor_port
);


SensorStatus sensor_application_read(
    Sensor_Application *application,
    TemperatureData *data,
    TemperatureLevel *temperature_level
);


#endif /* SENSOR_APPLICATION_H */
