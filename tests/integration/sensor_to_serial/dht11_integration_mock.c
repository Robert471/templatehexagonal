#include <stdint.h>
#include <string.h>

#include "../../../ports_and_adapters/ports/hardware/dht11/dht11_port.h"


/* ============================================================
 * DHT11 Integration Mock
 * ============================================================ */

typedef struct
{
    Dht11_PinState sequence[128];

    uint16_t sequence_length;
    uint16_t sequence_index;

    uint32_t set_mode_call_count;
    uint32_t write_call_count;
    uint32_t read_call_count;
    uint32_t delay_call_count;

} Dht11_Integration_Mock;


static Dht11_Integration_Mock mock;


/* ============================================================
 * Agregar un estado a la secuencia
 * ============================================================ */

static void sequence_push(
    Dht11_PinState state
)
{
    if (mock.sequence_length >= 128U)
    {
        return;
    }

    mock.sequence[
        mock.sequence_length
    ] = state;

    mock.sequence_length++;
}


/* ============================================================
 * Agregar un byte DHT11
 *
 * Para cada bit el Adapter hace:
 *
 * 1. Esperar HIGH
 * 2. delay_us(30)
 * 3. read()
 * 4. Esperar LOW
 *
 * Por lo tanto:
 *
 * HIGH -> valor del bit -> LOW
 * ============================================================ */

static void append_byte(
    uint8_t value
)
{
    uint8_t bit;

    for (bit = 8U; bit > 0U; bit--)
    {
        /*
         * Inicio del bit.
         */
        sequence_push(
            DHT11_PIN_HIGH
        );


        /*
         * Valor del bit.
         */
        if ((value & ((uint8_t)1U << (bit - 1U))) != 0U)
        {
            sequence_push(
                DHT11_PIN_HIGH
            );
        }
        else
        {
            sequence_push(
                DHT11_PIN_LOW
            );
        }


        /*
         * Fin del bit.
         */
        sequence_push(
            DHT11_PIN_LOW
        );
    }
}


/* ============================================================
 * Inicialización
 *
 * Datos simulados:
 *
 * Humedad       = 65
 * Humedad dec.  = 0
 * Temperatura   = 28
 * Temperatura d.= 0
 * Checksum      = 93
 *
 * 65 + 0 + 28 + 0 = 93
 * ============================================================ */

void dht11_integration_mock_init(void)
{
    memset(
        &mock,
        0,
        sizeof(mock)
    );


    /*
     * Respuesta inicial del DHT11.
     *
     * LOW
     * HIGH
     * LOW
     */

    sequence_push(
        DHT11_PIN_LOW
    );

    sequence_push(
        DHT11_PIN_HIGH
    );

    sequence_push(
        DHT11_PIN_LOW
    );


    /*
     * 40 bits.
     */

    append_byte(65U);

    append_byte(0U);

    append_byte(28U);

    append_byte(0U);

    append_byte(93U);
}


/* ============================================================
 * DHT11 Port callbacks
 * ============================================================ */

static void mock_set_mode(
    void *context,
    Dht11_PinMode mode
)
{
    (void)context;
    (void)mode;

    mock.set_mode_call_count++;
}


static void mock_write(
    void *context,
    Dht11_PinState state
)
{
    (void)context;
    (void)state;

    mock.write_call_count++;
}


static Dht11_PinState mock_read(
    void *context
)
{
    (void)context;

    mock.read_call_count++;


    if (mock.sequence_index <
        mock.sequence_length)
    {
        return mock.sequence[
            mock.sequence_index++
        ];
    }


    /*
     * Si el Adapter intenta leer
     * más allá de la secuencia,
     * dejamos la línea en LOW.
     */

    return DHT11_PIN_LOW;
}


static void mock_delay_us(
    void *context,
    uint32_t microseconds
)
{
    (void)context;
    (void)microseconds;

    mock.delay_call_count++;
}


/* ============================================================
 * Obtener Dht11_Port
 * ============================================================ */

const Dht11_Port *
dht11_integration_mock_get_port(void)
{
    static Dht11_Port port;


    port.gpio_context = &mock;
    port.timer_context = &mock;

    port.set_mode = mock_set_mode;

    port.write = mock_write;

    port.read = mock_read;

    port.delay_us = mock_delay_us;


    return &port;
}


/* ============================================================
 * Estadísticas del Mock
 * ============================================================ */

uint32_t dht11_integration_mock_get_set_mode_calls(void)
{
    return mock.set_mode_call_count;
}


uint32_t dht11_integration_mock_get_write_calls(void)
{
    return mock.write_call_count;
}


uint32_t dht11_integration_mock_get_read_calls(void)
{
    return mock.read_call_count;
}


uint32_t dht11_integration_mock_get_delay_calls(void)
{
    return mock.delay_call_count;
}