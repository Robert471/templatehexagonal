#include <stddef.h>

#include "sensor_port_mock.h"

void sensor_port_mock_init(Sensor_Port_Mock *mock)
{
    if (mock == NULL)
    {
        return;
    }
    mock->expected_status = SENSOR_OK;
    mock->expected_data.temperature = 0;
    mock->expected_data.humidity = 0;
    mock->read_call_count = 0;
}

SensorStatus sensor_port_mock_read(void *context, TemperatureData *data)
{
    Sensor_Port_Mock *mock = (Sensor_Port_Mock *)context;

    if (mock == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    mock->read_call_count++;

    if (data == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    if (mock->expected_status != SENSOR_OK)
    {
        return mock->expected_status;
    }

    data->temperature = mock->expected_data.temperature;
    data->humidity = mock->expected_data.humidity;

    return SENSOR_OK;
}

const Sensor_Port *sensor_port_mock_get_port(Sensor_Port_Mock *mock)
{
    static Sensor_Port port;
    port.context = mock;
    port.read = sensor_port_mock_read;
    return &port;
}