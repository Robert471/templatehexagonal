#include <stdio.h>
#include <assert.h>

#include "../../../../ports_and_adapters/application/inc/sensor_application.h"
#include "sensor_port_mock.h"


static void test_sensor_application_init(void)
{
    Sensor_Application application;
    Sensor_Port_Mock mock;

    sensor_port_mock_init(&mock);

    const Sensor_Port *sensor_port =
        sensor_port_mock_get_port(&mock);

    sensor_application_init(
        &application,
        sensor_port
    );

    assert(application.sensor_port.read ==
           sensor_port->read);

    assert(application.sensor_port.context ==
           sensor_port->context);
}


static void test_sensor_application_read_success(void)
{
    Sensor_Application application;
    Sensor_Port_Mock mock;

    sensor_port_mock_init(&mock);

    mock.expected_status = SENSOR_OK;
    mock.expected_data.temperature = 28;
    mock.expected_data.humidity = 65;

    const Sensor_Port *sensor_port =
        sensor_port_mock_get_port(&mock);

    sensor_application_init(
        &application,
        sensor_port
    );

    TemperatureData data;
    TemperatureLevel temperature_level;

    SensorStatus status =
        sensor_application_read(
            &application,
            &data,
            &temperature_level
        );

    assert(status == SENSOR_OK);

    assert(data.temperature == 28);
    assert(data.humidity == 65);

    assert(mock.read_call_count == 1);
}


static void test_sensor_application_read_error(void)
{
    Sensor_Application application;
    Sensor_Port_Mock mock;

    sensor_port_mock_init(&mock);

    mock.expected_status =
        SENSOR_ERROR_TIMEOUT;

    const Sensor_Port *sensor_port =
        sensor_port_mock_get_port(&mock);

    sensor_application_init(
        &application,
        sensor_port
    );

    TemperatureData data;
    TemperatureLevel temperature_level;

    SensorStatus status =
        sensor_application_read(
            &application,
            &data,
            &temperature_level
        );

    assert(status == SENSOR_ERROR_TIMEOUT);

    assert(mock.read_call_count == 1);
}


static void test_sensor_application_null_application(void)
{
    Sensor_Port_Mock mock;

    sensor_port_mock_init(&mock);

    TemperatureData data;
    TemperatureLevel temperature_level;

    SensorStatus status =
        sensor_application_read(
            NULL,
            &data,
            &temperature_level
        );

    assert(status == SENSOR_ERROR_INVALID_PARAMETER);
}


static void test_sensor_application_null_data(void)
{
    Sensor_Application application;
    Sensor_Port_Mock mock;

    sensor_port_mock_init(&mock);

    const Sensor_Port *sensor_port =
        sensor_port_mock_get_port(&mock);

    sensor_application_init(
        &application,
        sensor_port
    );

    TemperatureLevel temperature_level;

    SensorStatus status =
        sensor_application_read(
            &application,
            NULL,
            &temperature_level
        );

    assert(status == SENSOR_ERROR_INVALID_PARAMETER);
}


int main(void)
{
    printf("Running sensor application tests...\n");

    test_sensor_application_init();
    printf("[PASS] sensor_application_init\n");

    test_sensor_application_read_success();
    printf("[PASS] sensor_application_read_success\n");

    test_sensor_application_read_error();
    printf("[PASS] sensor_application_read_error\n");

    test_sensor_application_null_application();
    printf("[PASS] sensor_application_null_application\n");

    test_sensor_application_null_data();
    printf("[PASS] sensor_application_null_data\n");

    printf("\nAll sensor application tests passed.\n");

    return 0;
}