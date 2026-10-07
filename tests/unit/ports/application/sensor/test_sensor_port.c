#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../../../../ports_and_adapters/ports/application/sensor/sensor_port.h"


typedef struct
{
    TemperatureData data;
    int read_called;

} Sensor_Port_Test_Context;


/* =========================================================
 * Fake de read()
 * ========================================================= */

static SensorStatus test_read(
    void *context,
    TemperatureData *data
)
{
    Sensor_Port_Test_Context *test_context =
        (Sensor_Port_Test_Context *)context;

    assert(test_context != NULL);
    assert(data != NULL);

    *data = test_context->data;

    test_context->read_called++;

    return SENSOR_OK;
}


/* =========================================================
 * TEST
 * read() entrega correctamente la medición
 * ========================================================= */

static void test_sensor_port_read(void)
{
    Sensor_Port_Test_Context context;

    memset(
        &context,
        0,
        sizeof(context)
    );

    context.data.temperature = 28;
    context.data.humidity = 65;


    Sensor_Port port =
    {
        .context = &context,
        .read = test_read
    };


    TemperatureData data;

    memset(
        &data,
        0,
        sizeof(data)
    );


    SensorStatus status =
        port.read(
            port.context,
            &data
        );


    assert(
        status == SENSOR_OK
    );

    assert(
        context.read_called == 1
    );

    assert(
        data.temperature == 28
    );

    assert(
        data.humidity == 65
    );


    printf(
        "[PASS] sensor_port_read\n"
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" SENSOR PORT UNIT TESTS\n");
    printf("========================================\n\n");


    test_sensor_port_read();


    printf("\n");
    printf("========================================\n");
    printf(" ALL SENSOR PORT TESTS PASSED\n");
    printf("========================================\n");

    return 0;
}