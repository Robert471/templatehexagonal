#include <stdio.h>
#include <assert.h>

#include "../../../../ports_and_adapters/application/inc/comm_application.h"
#include "comm_port_mock.h"


static void test_comm_application_init(void)
{
    Comm_Application application;
    Comm_Port_Mock mock;

    comm_port_mock_init(&mock);

    const Comm_Port *comm_port =
        comm_port_mock_get_port(&mock);

    comm_application_init(
        &application,
        comm_port
    );

    assert(application.comm_port.send_temperature ==
           comm_port->send_temperature);
    assert(application.comm_port.context ==
           comm_port->context);
}


static void test_comm_application_send_temperature_success(void)
{
    Comm_Application application;
    Comm_Port_Mock mock;

    comm_port_mock_init(&mock);

    mock.expected_status = COMM_OK;

    const Comm_Port *comm_port =
        comm_port_mock_get_port(&mock);

    comm_application_init(
        &application,
        comm_port
    );

    TemperatureData data;

    data.temperature = 28;
    data.humidity = 65;

    CommStatus status =
        comm_application_send_temperature(
            &application,
            &data
        );

    assert(status == COMM_OK);

    assert(mock.send_temperature_call_count == 1);

    assert(mock.last_data.temperature == 28);
    assert(mock.last_data.humidity == 65);
}


static void test_comm_application_send_temperature_error(void)
{
    Comm_Application application;
    Comm_Port_Mock mock;

    comm_port_mock_init(&mock);

    mock.expected_status = COMM_ERROR;

    const Comm_Port *comm_port =
        comm_port_mock_get_port(&mock);

    comm_application_init(
        &application,
        comm_port
    );

    TemperatureData data;

    data.temperature = 30;
    data.humidity = 70;

    CommStatus status =
        comm_application_send_temperature(
            &application,
            &data
        );

    assert(status == COMM_ERROR);

    assert(mock.send_temperature_call_count == 1);
}


static void test_comm_application_null_application(void)
{
    Comm_Port_Mock mock;

    comm_port_mock_init(&mock);

    TemperatureData data;

    data.temperature = 25;
    data.humidity = 50;

    CommStatus status =
        comm_application_send_temperature(
            NULL,
            &data
        );

    assert(status == COMM_ERROR_INVALID_PARAMETER);

    assert(mock.send_temperature_call_count == 0);
}


static void test_comm_application_null_data(void)
{
    Comm_Application application;
    Comm_Port_Mock mock;

    comm_port_mock_init(&mock);

    const Comm_Port *comm_port =
        comm_port_mock_get_port(&mock);

    comm_application_init(
        &application,
        comm_port
    );

    CommStatus status =
        comm_application_send_temperature(
            &application,
            NULL
        );

    assert(status == COMM_ERROR_INVALID_PARAMETER);

    assert(mock.send_temperature_call_count == 0);
}


int main(void)
{
    printf("Running communication application tests...\n");

    test_comm_application_init();
    printf("[PASS] comm_application_init\n");

    test_comm_application_send_temperature_success();
    printf("[PASS] comm_application_send_temperature_success\n");

    test_comm_application_send_temperature_error();
    printf("[PASS] comm_application_send_temperature_error\n");

    test_comm_application_null_application();
    printf("[PASS] comm_application_null_application\n");

    test_comm_application_null_data();
    printf("[PASS] comm_application_null_data\n");

    printf("\nAll communication application tests passed.\n");

    return 0;
}