#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../../../../../ports_and_adapters/ports/hardware/serial/serial_port.h"


typedef struct
{
    uint8_t buffer[128];
    uint16_t length;
    uint32_t send_called;

} Serial_Port_Test_Context;


/* =========================================================
 * Fake de send()
 * ========================================================= */

static SerialStatus test_send(
    void *context,
    const uint8_t *data,
    uint16_t length
)
{
    Serial_Port_Test_Context *test_context =
        (Serial_Port_Test_Context *)context;

    assert(test_context != NULL);
    assert(data != NULL);

    assert(length <= sizeof(test_context->buffer));

    memcpy(
        test_context->buffer,
        data,
        length
    );

    test_context->length = length;
    test_context->send_called++;

    return SERIAL_OK;
}


/* =========================================================
 * TEST
 * Verificar que Serial_Port transmite correctamente
 * ========================================================= */

static void test_serial_port_send(void)
{
    Serial_Port_Test_Context context;

    memset(
        &context,
        0,
        sizeof(context)
    );


    Serial_Port port =
    {
        .context = &context,
        .send = test_send
    };


    uint8_t message[] =
        "Temperature: 28 C\r\n";


    port.send(
        port.context,
        message,
        (uint16_t)(sizeof(message) - 1U)
    );


    assert(
        context.send_called == 1
    );


    assert(
        context.length ==
        (uint16_t)(sizeof(message) - 1U)
    );


    assert(
        memcmp(
            context.buffer,
            message,
            sizeof(message) - 1U
        ) == 0
    );


    printf(
        "[PASS] serial_port_send\n"
    );
}


/* =========================================================
 * TEST
 * Verificar que el context llega correctamente
 * ========================================================= */

static void test_serial_port_context(void)
{
    Serial_Port_Test_Context context;

    memset(
        &context,
        0,
        sizeof(context)
    );


    Serial_Port port =
    {
        .context = &context,
        .send = test_send
    };


    uint8_t message[] =
        "Hello\r\n";


    port.send(
        port.context,
        message,
        (uint16_t)(sizeof(message) - 1U)
    );


    assert(
        context.send_called == 1
    );


    assert(
        port.context == &context
    );


    printf(
        "[PASS] serial_port_context\n"
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" SERIAL PORT UNIT TESTS\n");
    printf("========================================\n\n");


    test_serial_port_send();

    test_serial_port_context();


    printf("\n");
    printf("========================================\n");
    printf(" ALL SERIAL PORT TESTS PASSED\n");
    printf("========================================\n");


    return 0;
}