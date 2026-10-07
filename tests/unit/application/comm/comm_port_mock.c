#include <stddef.h>

#include "comm_port_mock.h"

void comm_port_mock_init(
    Comm_Port_Mock *mock
)
{
    if (mock == NULL)
    {
        return;
    }

    mock->expected_status = COMM_OK;

    mock->last_data.temperature = 0;
    mock->last_data.humidity = 0;

    mock->send_temperature_call_count = 0;
}


CommStatus comm_port_mock_send_temperature(
    void *context,
    const TemperatureData *data
)
{
    Comm_Port_Mock *mock =
        (Comm_Port_Mock *)context;

    if (mock == NULL)
    {
        return COMM_ERROR;
    }

    mock->send_temperature_call_count++;

    if (data == NULL)
    {
        return COMM_ERROR;
    }

    mock->last_data.temperature =
        data->temperature;

    mock->last_data.humidity =
        data->humidity;

    return mock->expected_status;
}


const Comm_Port *comm_port_mock_get_port(
    Comm_Port_Mock *mock
)
{
    static Comm_Port port;

    port.context = mock;
    port.send_temperature =
        comm_port_mock_send_temperature;

    return &port;
}