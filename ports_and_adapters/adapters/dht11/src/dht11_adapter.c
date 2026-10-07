#include "../inc/dht11_adapter.h"

#include <stddef.h>
#include <stdint.h>

/*
 * DHT11 protocol timing.
 *
 * These values are intentionally kept identical to the original,
 * HIL-tested implementation. The SOLID refactor must not change
 * the timing behavior of the protocol.
 */
#define DHT11_START_LOW_US   18000U
#define DHT11_START_HIGH_US     40U
#define DHT11_TIMEOUT_US        100U
#define DHT11_SAMPLE_US          30U

static SensorStatus dht11_validate_port(
    const Dht11_Adapter *adapter)
{
    if (adapter == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    if ((adapter->port.gpio_context == NULL) ||
        (adapter->port.timer_context == NULL) ||
        (adapter->port.set_mode == NULL) ||
        (adapter->port.write == NULL) ||
        (adapter->port.read == NULL) ||
        (adapter->port.delay_us == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    return SENSOR_OK;
}

/*
 * Wait for a GPIO level without changing the timer implementation.
 *
 * The Timer Port remains an injected dependency. Therefore the
 * protocol does not know whether the concrete implementation uses
 * TIM3, a simulator, or a test double.
 */
static SensorStatus dht11_wait_level(
    const Dht11_Adapter *adapter,
    Dht11_PinState level,
    uint32_t timeout_us)
{
    uint32_t elapsed = 0U;

    while (adapter->port.read(adapter->port.gpio_context) != level)
    {
        adapter->port.delay_us(
            adapter->port.timer_context,
            1U
        );

        elapsed++;

        if (elapsed >= timeout_us)
        {
            return SENSOR_ERROR_TIMEOUT;
        }
    }

    return SENSOR_OK;
}

static SensorStatus dht11_read_byte(
    const Dht11_Adapter *adapter,
    uint8_t *value)
{
    uint8_t byte = 0U;
    uint8_t bit_index;

    if (value == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    for (bit_index = 0U; bit_index < 8U; bit_index++)
    {
        /*
         * Each data bit begins with a HIGH pulse.
         */
        if (dht11_wait_level(
                adapter,
                DHT11_PIN_HIGH,
                DHT11_TIMEOUT_US) != SENSOR_OK)
        {
            return SENSOR_ERROR_TIMEOUT;
        }

        /*
         * The DHT11 encodes 0/1 using the duration of HIGH.
         * Sample approximately 30 us after the rising edge.
         */
        adapter->port.delay_us(
            adapter->port.timer_context,
            DHT11_SAMPLE_US
        );

        byte = (uint8_t)(byte << 1U);

        if (adapter->port.read(adapter->port.gpio_context) ==
            DHT11_PIN_HIGH)
        {
            byte = (uint8_t)(byte | 1U);
        }

        /*
         * Wait for the HIGH pulse to finish before reading
         * the next bit.
         */
        if (dht11_wait_level(
                adapter,
                DHT11_PIN_LOW,
                DHT11_TIMEOUT_US) != SENSOR_OK)
        {
            return SENSOR_ERROR_TIMEOUT;
        }
    }

    *value = byte;
    return SENSOR_OK;
}

static SensorStatus dht11_send_start_signal(
    const Dht11_Adapter *adapter)
{
    adapter->port.set_mode(
        adapter->port.gpio_context,
        DHT11_PIN_OUTPUT
    );

    adapter->port.write(
        adapter->port.gpio_context,
        DHT11_PIN_LOW
    );

    adapter->port.delay_us(
        adapter->port.timer_context,
        DHT11_START_LOW_US
    );

    adapter->port.write(
        adapter->port.gpio_context,
        DHT11_PIN_HIGH
    );

    adapter->port.delay_us(
        adapter->port.timer_context,
        DHT11_START_HIGH_US
    );

    /*
     * Release the bus so that the DHT11 can drive it.
     */
    adapter->port.set_mode(
        adapter->port.gpio_context,
        DHT11_PIN_INPUT
    );

    return SENSOR_OK;
}

static SensorStatus dht11_wait_response(
    const Dht11_Adapter *adapter)
{
    if (dht11_wait_level(
            adapter,
            DHT11_PIN_LOW,
            DHT11_TIMEOUT_US) != SENSOR_OK)
    {
        return SENSOR_ERROR_TIMEOUT;
    }

    if (dht11_wait_level(
            adapter,
            DHT11_PIN_HIGH,
            DHT11_TIMEOUT_US) != SENSOR_OK)
    {
        return SENSOR_ERROR_TIMEOUT;
    }

    if (dht11_wait_level(
            adapter,
            DHT11_PIN_LOW,
            DHT11_TIMEOUT_US) != SENSOR_OK)
    {
        return SENSOR_ERROR_TIMEOUT;
    }

    return SENSOR_OK;
}

static SensorStatus dht11_read_payload(
    const Dht11_Adapter *adapter,
    uint8_t raw[5])
{
    uint8_t index;

    for (index = 0U; index < 5U; index++)
    {
        if (dht11_read_byte(adapter, &raw[index]) != SENSOR_OK)
        {
            return SENSOR_ERROR_TIMEOUT;
        }
    }

    return SENSOR_OK;
}

static SensorStatus dht11_validate_checksum(
    const uint8_t raw[5])
{
    uint16_t checksum_sum;
    uint8_t checksum;

    checksum_sum = (uint16_t)raw[0];
    checksum_sum = (uint16_t)(
        checksum_sum + (uint16_t)raw[1]
    );
    checksum_sum = (uint16_t)(
        checksum_sum + (uint16_t)raw[2]
    );
    checksum_sum = (uint16_t)(
        checksum_sum + (uint16_t)raw[3]
    );

    checksum = (uint8_t)checksum_sum;

    if (checksum != raw[4])
    {
        return SENSOR_ERROR_CHECKSUM;
    }

    return SENSOR_OK;
}

void dht11_adapter_init(
    Dht11_Adapter *adapter,
    const Dht11_Port *port)
{
    if ((adapter == NULL) || (port == NULL))
    {
        return;
    }

    adapter->port = *port;
}

SensorStatus dht11_adapter_read(
    void *context,
    TemperatureData *data)
{
    Dht11_Adapter *adapter = (Dht11_Adapter *)context;
    uint8_t raw[5] = {0U, 0U, 0U, 0U, 0U};

    if ((adapter == NULL) || (data == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    if (dht11_validate_port(adapter) != SENSOR_OK)
    {
        return SENSOR_ERROR_INVALID_PARAMETER;
    }

    /*
     * The adapter owns the DHT11 protocol.
     * Hardware access is delegated exclusively to Dht11_Port.
     */
    (void)dht11_send_start_signal(adapter);

    if (dht11_wait_response(adapter) != SENSOR_OK)
    {
        return SENSOR_ERROR_TIMEOUT;
    }

    if (dht11_read_payload(adapter, raw) != SENSOR_OK)
    {
        return SENSOR_ERROR_TIMEOUT;
    }

    if (dht11_validate_checksum(raw) != SENSOR_OK)
    {
        return SENSOR_ERROR_CHECKSUM;
    }

    /*
     * DHT11 integer mode:
     * raw[0] = humidity integer
     * raw[2] = temperature integer
     */
    data->humidity = raw[0];
    data->temperature = raw[2];

    return SENSOR_OK;
}
