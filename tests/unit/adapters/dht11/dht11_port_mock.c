#include "dht11_port_mock.h"

#include <stddef.h>
#include <string.h>


static void dht11_port_mock_set_mode(
    void *context,
    Dht11_PinMode mode)
{
    Dht11_Port_Mock *mock =
        (Dht11_Port_Mock *)context;

    if (mock == NULL)
    {
        return;
    }

    mock->mode = mode;
    mock->set_mode_call_count++;
}


static void dht11_port_mock_write(
    void *context,
    Dht11_PinState state)
{
    Dht11_Port_Mock *mock =
        (Dht11_Port_Mock *)context;

    if (mock == NULL)
    {
        return;
    }

    mock->state = state;
    mock->write_call_count++;
}


static Dht11_PinState dht11_port_mock_read(
    void *context)
{
    Dht11_Port_Mock *mock =
        (Dht11_Port_Mock *)context;

    if (mock == NULL)
    {
        return DHT11_PIN_LOW;
    }

    mock->read_call_count++;

    if (mock->sequence_index < mock->sequence_length)
    {
        return mock->sequence[
            mock->sequence_index++
        ];
    }

    return DHT11_PIN_LOW;
}


static void dht11_port_mock_delay_us(
    void *context,
    uint32_t microseconds)
{
    Dht11_Port_Mock *mock =
        (Dht11_Port_Mock *)context;

    if (mock == NULL)
    {
        return;
    }

    mock->last_delay_us = microseconds;
    mock->delay_call_count++;
}


void dht11_port_mock_init(
    Dht11_Port_Mock *mock)
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

    mock->mode = DHT11_PIN_INPUT;
    mock->state = DHT11_PIN_LOW;

    mock->port.gpio_context = mock;
    mock->port.timer_context = mock;
    mock->port.set_mode = dht11_port_mock_set_mode;
    mock->port.write = dht11_port_mock_write;
    mock->port.read = dht11_port_mock_read;
    mock->port.delay_us = dht11_port_mock_delay_us;
}


void dht11_port_mock_set_sequence(
    Dht11_Port_Mock *mock,
    const Dht11_PinState *sequence,
    uint16_t length)
{
    if ((mock == NULL) ||
        (sequence == NULL) ||
        (length <= 0))
    {
        return;
    }

    if (length > 128)
    {
        length = 128;
    }

    memcpy(
        mock->sequence,
        sequence,
        (size_t)length * sizeof(sequence[0])
    );

    mock->sequence_length = length;
    mock->sequence_index = 0;
}


const Dht11_Port *dht11_port_mock_get_port(
    Dht11_Port_Mock *mock)
{
    if (mock == NULL)
    {
        return NULL;
    }

    return &mock->port;
}
