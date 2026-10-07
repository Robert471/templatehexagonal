#include "serial_port_mock.h"

#include <stddef.h>
#include <string.h>


static SerialStatus serial_port_mock_send(
    void *context,
    const uint8_t *data,
    uint16_t size)
{
    Serial_Port_Mock *mock =
        (Serial_Port_Mock *)context;

    if ((mock == NULL) ||
        (data == NULL))
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }

    if (size > (uint16_t)sizeof(mock->buffer))
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }

    memcpy(
        mock->buffer,
        data,
        size
    );

    mock->size = size;
    mock->send_call_count++;

    return SERIAL_OK;
}


void serial_port_mock_init(
    Serial_Port_Mock *mock)
{
    if (mock == NULL)
    {
        return;
    }

    memset(
        mock,
        0,
        sizeof(*mock)
    );

    mock->port.context = mock;
    mock->port.send = serial_port_mock_send;
}


const Serial_Port *serial_port_mock_get_port(
    Serial_Port_Mock *mock)
{
    if (mock == NULL)
    {
        return NULL;
    }

    return &mock->port;
}
