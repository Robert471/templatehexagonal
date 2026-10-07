#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "../../../../../ports_and_adapters/ports/hardware/dht11/dht11_port.h"


typedef struct
{
    Dht11_PinMode last_mode;
    Dht11_PinState last_write;
    Dht11_PinState read_state;

    uint32_t last_delay;

    uint32_t set_mode_called;
    uint32_t write_called;
    uint32_t read_called;
    uint32_t delay_called;

} Dht11_Port_Test_Context;


/* =========================================================
 * Implementación fake del contrato
 * ========================================================= */

static void test_set_mode(
    void *context,
    Dht11_PinMode mode
)
{
    Dht11_Port_Test_Context *test_context =
        (Dht11_Port_Test_Context *)context;

    assert(test_context != NULL);

    test_context->last_mode = mode;
    test_context->set_mode_called++;
}


static void test_write(
    void *context,
    Dht11_PinState state
)
{
    Dht11_Port_Test_Context *test_context =
        (Dht11_Port_Test_Context *)context;

    assert(test_context != NULL);

    test_context->last_write = state;
    test_context->write_called++;
}


static Dht11_PinState test_read(
    void *context
)
{
    Dht11_Port_Test_Context *test_context =
        (Dht11_Port_Test_Context *)context;

    assert(test_context != NULL);

    test_context->read_called++;

    return test_context->read_state;
}


static void test_delay_us(
    void *context,
    uint32_t microseconds
)
{
    Dht11_Port_Test_Context *test_context =
        (Dht11_Port_Test_Context *)context;

    assert(test_context != NULL);

    test_context->last_delay = microseconds;
    test_context->delay_called++;
}


/* =========================================================
 * Inicialización del contexto
 * ========================================================= */

static void reset_context(
    Dht11_Port_Test_Context *context
)
{
    context->last_mode = DHT11_PIN_INPUT;
    context->last_write = DHT11_PIN_LOW;
    context->read_state = DHT11_PIN_LOW;

    context->last_delay = 0U;

    context->set_mode_called = 0;
    context->write_called = 0;
    context->read_called = 0;
    context->delay_called = 0;
}


/* =========================================================
 * Crear port de prueba
 * ========================================================= */

static Dht11_Port create_test_port(
    Dht11_Port_Test_Context *context
)
{
    Dht11_Port port =
    {
        .gpio_context = context,
        .timer_context = context,

        .set_mode = test_set_mode,
        .write = test_write,
        .read = test_read,
        .delay_us = test_delay_us
    };

    return port;
}


/* =========================================================
 * TEST
 * set_mode INPUT
 * ========================================================= */

static void test_dht11_port_set_mode_input(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    Dht11_Port port =
        create_test_port(&context);

    port.set_mode(
        port.gpio_context,
        DHT11_PIN_INPUT
    );

    assert(
        context.set_mode_called == 1
    );

    assert(
        context.last_mode == DHT11_PIN_INPUT
    );

    printf(
        "[PASS] dht11_port_set_mode_input\n"
    );
}


/* =========================================================
 * TEST
 * set_mode OUTPUT
 * ========================================================= */

static void test_dht11_port_set_mode_output(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    Dht11_Port port =
        create_test_port(&context);

    port.set_mode(
        port.gpio_context,
        DHT11_PIN_OUTPUT
    );

    assert(
        context.set_mode_called == 1
    );

    assert(
        context.last_mode == DHT11_PIN_OUTPUT
    );

    printf(
        "[PASS] dht11_port_set_mode_output\n"
    );
}


/* =========================================================
 * TEST
 * write LOW
 * ========================================================= */

static void test_dht11_port_write_low(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    Dht11_Port port =
        create_test_port(&context);

    port.write(
        port.gpio_context,
        DHT11_PIN_LOW
    );

    assert(
        context.write_called == 1
    );

    assert(
        context.last_write == DHT11_PIN_LOW
    );

    printf(
        "[PASS] dht11_port_write_low\n"
    );
}


/* =========================================================
 * TEST
 * write HIGH
 * ========================================================= */

static void test_dht11_port_write_high(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    Dht11_Port port =
        create_test_port(&context);

    port.write(
        port.gpio_context,
        DHT11_PIN_HIGH
    );

    assert(
        context.write_called == 1
    );

    assert(
        context.last_write == DHT11_PIN_HIGH
    );

    printf(
        "[PASS] dht11_port_write_high\n"
    );
}


/* =========================================================
 * TEST
 * read LOW
 * ========================================================= */

static void test_dht11_port_read_low(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    context.read_state = DHT11_PIN_LOW;

    Dht11_Port port =
        create_test_port(&context);

    Dht11_PinState state =
        port.read(
            port.gpio_context
        );

    assert(
        context.read_called == 1
    );

    assert(
        state == DHT11_PIN_LOW
    );

    printf(
        "[PASS] dht11_port_read_low\n"
    );
}


/* =========================================================
 * TEST
 * read HIGH
 * ========================================================= */

static void test_dht11_port_read_high(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    context.read_state = DHT11_PIN_HIGH;

    Dht11_Port port =
        create_test_port(&context);

    Dht11_PinState state =
        port.read(
            port.gpio_context
        );

    assert(
        context.read_called == 1
    );

    assert(
        state == DHT11_PIN_HIGH
    );

    printf(
        "[PASS] dht11_port_read_high\n"
    );
}


/* =========================================================
 * TEST
 * delay_us
 * ========================================================= */

static void test_dht11_port_delay(void)
{
    Dht11_Port_Test_Context context;

    reset_context(&context);

    Dht11_Port port =
        create_test_port(&context);

    port.delay_us(
        port.timer_context,
        18000U
    );

    assert(
        context.delay_called == 1
    );

    assert(
        context.last_delay == 18000U
    );

    printf(
        "[PASS] dht11_port_delay\n"
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" DHT11 PORT UNIT TESTS\n");
    printf("========================================\n\n");

    test_dht11_port_set_mode_input();

    test_dht11_port_set_mode_output();

    test_dht11_port_write_low();

    test_dht11_port_write_high();

    test_dht11_port_read_low();

    test_dht11_port_read_high();

    test_dht11_port_delay();

    printf("\n");
    printf("========================================\n");
    printf(" ALL DHT11 PORT TESTS PASSED\n");
    printf("========================================\n");

    return 0;
}