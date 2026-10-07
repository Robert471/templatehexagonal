#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../../../../ports_and_adapters/ports/application/comm/comm_port.h"


typedef struct
{
    TemperatureData received_data;
    int send_called;

} Comm_Port_Test_Context;


/* =========================================================
 * Fake de send_temperature()
 * ========================================================= */

static CommStatus test_send_temperature(
    void *context,
    const TemperatureData *data
)
{
    Comm_Port_Test_Context *test_context =
        (Comm_Port_Test_Context *)context;

    assert(test_context != NULL);
    assert(data != NULL);

    test_context->received_data = *data;

    test_context->send_called++;

    return COMM_OK;
}


/* =========================================================
 * TEST
 * send_temperature() recibe correctamente la medición
 * ========================================================= */

static void test_comm_port_send_temperature(void)
{
    Comm_Port_Test_Context context;

    memset(
        &context,
        0,
        sizeof(context)
    );


    Comm_Port port =
    {
        .context = &context,
        .send_temperature = test_send_temperature
    };


    TemperatureData data;

    data.temperature = 28;
    data.humidity = 65;


    CommStatus status =
        port.send_temperature(
            port.context,
            &data
        );


    assert(
        status == COMM_OK
    );

    assert(
        context.send_called == 1
    );

    assert(
        context.received_data.temperature == 28
    );

    assert(
        context.received_data.humidity == 65
    );


    printf(
        "[PASS] comm_port_send_temperature\n"
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" COMM PORT UNIT TESTS\n");
    printf("========================================\n\n");


    test_comm_port_send_temperature();


    printf("\n");
    printf("========================================\n");
    printf(" ALL COMM PORT TESTS PASSED\n");
    printf("========================================\n");

    return 0;
}