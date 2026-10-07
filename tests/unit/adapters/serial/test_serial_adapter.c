#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../../../../ports_and_adapters/adapters/serial/inc/serial_adapter.h"
#include "serial_port_mock.h"


static void test_serial_adapter_init(void)
{
    Serial_Adapter adapter;
    Serial_Port_Mock mock;

    serial_port_mock_init(&mock);

    serial_adapter_init(
        &adapter,
        serial_port_mock_get_port(&mock)
    );

    assert(
        adapter.serial_port.context == &mock
    );

    assert(
        adapter.serial_port.send != NULL
    );

    printf(
        "[PASS] serial_adapter_init\n"
    );
}


static void test_serial_adapter_send_temperature(void)
{
    Serial_Adapter adapter;
    Serial_Port_Mock mock;
    TemperatureData data;

    serial_port_mock_init(&mock);

    serial_adapter_init(
        &adapter,
        serial_port_mock_get_port(&mock)
    );

    data.temperature = 28;
    data.humidity = 65;

    CommStatus status =
        serial_adapter_send_temperature(
            &adapter,
            &data
        );

    assert(status == COMM_OK);

    assert(mock.send_call_count == 1);

    assert(
        mock.size ==
        strlen("Temperature: 28 C\r\n")
    );

    assert(
        memcmp(
            mock.buffer,
            "Temperature: 28 C\r\n",
            mock.size
        ) == 0
    );

    printf(
        "[PASS] serial_adapter_send_temperature\n"
    );
}


static void test_serial_adapter_temperature_value(void)
{
    Serial_Adapter adapter;
    Serial_Port_Mock mock;
    TemperatureData data;

    serial_port_mock_init(&mock);

    serial_adapter_init(
        &adapter,
        serial_port_mock_get_port(&mock)
    );

    data.temperature = 0;
    data.humidity = 99;

    CommStatus status =
        serial_adapter_send_temperature(
            &adapter,
            &data
        );

    assert(status == COMM_OK);

    assert(mock.send_call_count == 1);

    assert(
        memcmp(
            mock.buffer,
            "Temperature: 0 C\r\n",
            mock.size
        ) == 0
    );

    printf(
        "[PASS] serial_adapter_temperature_value\n"
    );
}


static void test_serial_adapter_null_adapter(void)
{
    TemperatureData data;

    data.temperature = 28;
    data.humidity = 65;

    CommStatus status =
        serial_adapter_send_temperature(
            NULL,
            &data
        );

    assert(
        status == COMM_ERROR_INVALID_PARAMETER
    );

    printf(
        "[PASS] serial_adapter_null_adapter\n"
    );
}


static void test_serial_adapter_null_data(void)
{
    Serial_Adapter adapter;
    Serial_Port_Mock mock;

    serial_port_mock_init(&mock);

    serial_adapter_init(
        &adapter,
        serial_port_mock_get_port(&mock)
    );

    CommStatus status =
        serial_adapter_send_temperature(
            &adapter,
            NULL
        );

    assert(
        status == COMM_ERROR_INVALID_PARAMETER
    );

    assert(
        mock.send_call_count == 0
    );

    printf(
        "[PASS] serial_adapter_null_data\n"
    );
}


static void test_serial_adapter_invalid_port(void)
{
    Serial_Adapter adapter;
    TemperatureData data;

    memset(
        &adapter,
        0,
        sizeof(adapter)
    );

    data.temperature = 28;
    data.humidity = 65;

    CommStatus status =
        serial_adapter_send_temperature(
            &adapter,
            &data
        );

    assert(
        status == COMM_ERROR_INVALID_PARAMETER
    );

    printf(
        "[PASS] serial_adapter_invalid_port\n"
    );
}


int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" SERIAL ADAPTER UNIT TESTS\n");
    printf("========================================\n\n");

    test_serial_adapter_init();
    test_serial_adapter_send_temperature();
    test_serial_adapter_temperature_value();
    test_serial_adapter_null_adapter();
    test_serial_adapter_null_data();
    test_serial_adapter_invalid_port();

    printf("\n");
    printf("========================================\n");
    printf(" ALL SERIAL ADAPTER TESTS PASSED\n");
    printf("========================================\n");

    return 0;
}
