#include <stddef.h>

#include "../inc/sensor_application.h"


void sensor_application_init(
    Sensor_Application *application,
    const Sensor_Port *sensor_port)
{
    if ((application == NULL) ||
        (sensor_port == NULL))
    {
        return;
    }

    application->sensor_port = *sensor_port;
}


SensorStatus sensor_application_read(
    Sensor_Application *application,
    TemperatureData *data,
    TemperatureLevel *temperature_level)
{
    SensorStatus status;


    if ((application == NULL) ||
        (data == NULL) ||
        (temperature_level == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }


    if (application->sensor_port.read == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }


    status = application->sensor_port.read(
        application->sensor_port.context,
        data
    );


    if (status != SENSOR_OK)
    {
        return status;
    }


    *temperature_level =
        temperature_classify(
            data->temperature
        );


    return SENSOR_OK;
}
