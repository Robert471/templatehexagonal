#include "../inc/serial_adapter.h"

#include <stddef.h>

#define SERIAL_TEMPERATURE_BUFFER_SIZE    64U
#define SERIAL_TEMPERATURE_MIN_BUFFER     21U

static CommStatus serial_adapter_validate(
    const Serial_Adapter *adapter,
    const TemperatureData *data)
{
    if ((adapter == NULL) || (data == NULL))
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    if (adapter->serial_port.send == NULL)
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    return COMM_OK;
}

static CommStatus serial_adapter_append_text(
    uint8_t *buffer,
    uint16_t buffer_size,
    uint16_t *index,
    const uint8_t *text,
    uint16_t text_length)
{
    uint16_t i;

    if ((buffer == NULL) ||
        (index == NULL) ||
        (text == NULL))
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    if ((*index > buffer_size) ||
        (text_length > (uint16_t)(buffer_size - *index)))
    {
        return COMM_ERROR;
    }

    for (i = 0U; i < text_length; i++)
    {
        buffer[*index] = text[i];
        *index = (uint16_t)(*index + 1U);
    }

    return COMM_OK;
}

/*
 * Converts an unsigned 32-bit integer to decimal text.
 *
 * Examples:
 *     0          -> "0"
 *     5          -> "5"
 *     28         -> "28"
 *     255        -> "255"
 *     1000       -> "1000"
 *     65535      -> "65535"
 *     4294967295 -> "4294967295"
 */
static CommStatus serial_adapter_uint32_to_text(
    uint32_t value,
    uint8_t *buffer,
    uint16_t buffer_size,
    uint16_t *length)
{
    uint8_t digits[10U];
    uint8_t digit_count = 0U;
    uint8_t digit;
    uint8_t index;
    uint32_t quotient;

    if ((buffer == NULL) ||
        (length == NULL) ||
        (buffer_size == 0U))
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    do
    {
        quotient = value / 10UL;
        digit = (uint8_t)(value % 10UL);

        digits[digit_count] = digit;
        digit_count = (uint8_t)(digit_count + 1U);

        value = quotient;
    }
    while (value > 0UL);

    if ((uint16_t)digit_count > buffer_size)
    {
        return COMM_ERROR;
    }

    for (index = 0U; index < digit_count; index++)
    {
        digit = digits[(uint8_t)(digit_count - index - 1U)];
        buffer[index] = (uint8_t)('0' + digit);
    }

    *length = (uint16_t)digit_count;

    return COMM_OK;
}

static CommStatus serial_adapter_format_temperature(
    const TemperatureData *data,
    uint8_t *buffer,
    uint16_t buffer_size,
    uint16_t *length)
{
    static const uint8_t label[] = "Temperature: ";
    static const uint8_t suffix[] = " C\r\n";

    uint16_t index = 0U;
    uint16_t temperature_length = 0U;
    CommStatus status;

    if ((data == NULL) ||
        (buffer == NULL) ||
        (length == NULL) ||
        (buffer_size == 0U))
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    if (buffer_size < SERIAL_TEMPERATURE_MIN_BUFFER)
    {
        return COMM_ERROR;
    }

    status = serial_adapter_append_text(
        buffer,
        buffer_size,
        &index,
        label,
        (uint16_t)(sizeof(label) - 1U));

    if (status != COMM_OK)
    {
        return status;
    }

    status = serial_adapter_uint32_to_text(
        (uint32_t)data->temperature,
        &buffer[index],
        (uint16_t)(buffer_size - index),
        &temperature_length);

    if (status != COMM_OK)
    {
        return status;
    }

    index = (uint16_t)(index + temperature_length);

    status = serial_adapter_append_text(
        buffer,
        buffer_size,
        &index,
        suffix,
        (uint16_t)(sizeof(suffix) - 1U));

    if (status != COMM_OK)
    {
        return status;
    }

    if (index < buffer_size)
    {
        buffer[index] = 0U;
    }

    *length = index;

    return COMM_OK;
}

void serial_adapter_init(
    Serial_Adapter *adapter,
    const Serial_Port *serial_port)
{
    if ((adapter == NULL) || (serial_port == NULL))
    {
        return;
    }

    adapter->serial_port = *serial_port;
}

CommStatus serial_adapter_send_temperature(
    void *context,
    const TemperatureData *data)
{
    Serial_Adapter *adapter = (Serial_Adapter *)context;
    uint8_t buffer[SERIAL_TEMPERATURE_BUFFER_SIZE];
    uint16_t length = 0U;
    SerialStatus serial_status;

    if (serial_adapter_validate(adapter, data) != COMM_OK)
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    if (serial_adapter_format_temperature(
            data,
            buffer,
            SERIAL_TEMPERATURE_BUFFER_SIZE,
            &length) != COMM_OK)
    {
        return COMM_ERROR;
    }

    serial_status = adapter->serial_port.send(
        adapter->serial_port.context,
        buffer,
        length);

    if (serial_status == SERIAL_OK)
    {
        return COMM_OK;
    }

    return COMM_ERROR;
}