#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "../../../../ports_and_adapters/adapters/dht11/inc/dht11_adapter.h"
#include "dht11_port_mock.h"



/*
 * =========================================================
 * Construcción de una secuencia DHT11
 * =========================================================
 *
 * El adapter espera:
 *
 * LOW
 * HIGH
 * LOW
 *
 * y después los 40 bits:
 *
 * 8 bits humedad entera
 * 8 bits humedad decimal
 * 8 bits temperatura entera
 * 8 bits temperatura decimal
 * 8 bits checksum
 *
 */

static void sequence_push(
    Dht11_PinState *sequence,
    uint16_t *length,
    Dht11_PinState state
)
{
    sequence[*length] = state;
    (*length)++;
}


static void append_byte(
    Dht11_PinState *sequence,
    uint16_t *length,
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
            sequence,
            length,
            DHT11_PIN_HIGH
        );

        /*
         * Valor del bit.
         *
         * El adapter hace:
         *
         * delay_us(30)
         * read()
         *
         * Por eso aquí devolvemos HIGH
         * para un 1 y LOW para un 0.
         */
        if (value & (1U << (bit - 1U)))
        {
            sequence_push(
                sequence,
                length,
                DHT11_PIN_HIGH
            );
        }
        else
        {
            sequence_push(
                sequence,
                length,
                DHT11_PIN_LOW
            );
        }

        /*
         * Fin del bit.
         */
        sequence_push(
            sequence,
            length,
            DHT11_PIN_LOW
        );
    }
}


static uint16_t build_dht11_sequence(
    Dht11_PinState *sequence,
    unsigned char humidity,
    unsigned char temperature,
    unsigned char checksum
)
{
    uint16_t length = 0U;

    /*
     * Respuesta inicial del DHT11.
     */
    sequence_push(
        sequence,
        &length,
        DHT11_PIN_LOW
    );

    sequence_push(
        sequence,
        &length,
        DHT11_PIN_HIGH
    );

    sequence_push(
        sequence,
        &length,
        DHT11_PIN_LOW
    );


    /*
     * Humedad entera.
     */
    append_byte(
        sequence,
        &length,
        humidity
    );


    /*
     * Humedad decimal.
     */
    append_byte(
        sequence,
        &length,
        0
    );


    /*
     * Temperatura entera.
     */
    append_byte(
        sequence,
        &length,
        temperature
    );


    /*
     * Temperatura decimal.
     */
    append_byte(
        sequence,
        &length,
        0
    );


    /*
     * Checksum.
     */
    append_byte(
        sequence,
        &length,
        checksum
    );


    return length;
}


/*
 * =========================================================
 * TEST 1
 * Lectura correcta
 * =========================================================
 *
 * Humedad     = 65
 * Temperatura = 28
 *
 * Checksum:
 *
 * 65 + 0 + 28 + 0 = 93
 *
 */

static void test_dht11_read_success(void)
{
    Dht11_Adapter adapter;
    Dht11_Port_Mock mock;

    Dht11_PinState sequence[128];

    TemperatureData data;

    uint16_t length;


    dht11_port_mock_init(
        &mock
    );


    length =
        build_dht11_sequence(
            sequence,
            65,
            28,
            93
        );


    dht11_port_mock_set_sequence(
        &mock,
        sequence,
        length
    );


    dht11_adapter_init(
        &adapter,
        dht11_port_mock_get_port(&mock)
    );


    SensorStatus status =
        dht11_adapter_read(
            &adapter,
            &data
        );


    assert(
        status == SENSOR_OK
    );


    assert(
        data.humidity == 65
    );


    assert(
        data.temperature == 28
    );


    printf(
        "[PASS] dht11_read_success\n"
    );
}


/*
 * =========================================================
 * TEST 2
 * Checksum incorrecto
 * =========================================================
 *
 * Datos:
 *
 * Humedad     = 65
 * Temperatura = 28
 *
 * Checksum correcto = 93
 *
 * Enviamos 94.
 */

static void test_dht11_checksum_error(void)
{
    Dht11_Adapter adapter;
    Dht11_Port_Mock mock;

    Dht11_PinState sequence[128];

    TemperatureData data;

    uint16_t length;


    dht11_port_mock_init(
        &mock
    );


    length =
        build_dht11_sequence(
            sequence,
            65,
            28,
            94
        );


    dht11_port_mock_set_sequence(
        &mock,
        sequence,
        length
    );


    dht11_adapter_init(
        &adapter,
        dht11_port_mock_get_port(&mock)
    );


    SensorStatus status =
        dht11_adapter_read(
            &adapter,
            &data
        );


    assert(
        status == SENSOR_ERROR_CHECKSUM
    );


    printf(
        "[PASS] dht11_checksum_error\n"
    );
}


/*
 * =========================================================
 * TEST 3
 * Timeout
 * =========================================================
 *
 * El sensor nunca genera LOW.
 */

static void test_dht11_timeout(void)
{
    Dht11_Adapter adapter;
    Dht11_Port_Mock mock;

    Dht11_PinState sequence[1];

    TemperatureData data;


    dht11_port_mock_init(
        &mock
    );


    /*
     * El adapter espera LOW.
     * El mock devuelve HIGH.
     */
    sequence[0] = DHT11_PIN_HIGH;


    dht11_port_mock_set_sequence(
        &mock,
        sequence,
        1
    );


    dht11_adapter_init(
        &adapter,
        dht11_port_mock_get_port(&mock)
    );


    SensorStatus status =
        dht11_adapter_read(
            &adapter,
            &data
        );


    assert(
        status == SENSOR_ERROR_TIMEOUT
    );


    printf(
        "[PASS] dht11_timeout\n"
    );
}


/*
 * =========================================================
 * TEST 4
 * data == NULL
 * =========================================================
 */

static void test_dht11_null_data(void)
{
    Dht11_Adapter adapter;
    Dht11_Port_Mock mock;


    dht11_port_mock_init(
        &mock
    );


    dht11_adapter_init(
        &adapter,
        dht11_port_mock_get_port(&mock)
    );


    SensorStatus status =
        dht11_adapter_read(
            &adapter,
            NULL
        );


    assert(
        status == SENSOR_ERROR_INVALID_PARAMETER
    );


    printf(
        "[PASS] dht11_null_data\n"
    );
}


/*
 * =========================================================
 * MAIN
 * =========================================================
 */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" DHT11 ADAPTER UNIT TESTS\n");
    printf("========================================\n\n");


    test_dht11_read_success();

    test_dht11_checksum_error();

    test_dht11_timeout();

    test_dht11_null_data();


    printf("\n");
    printf("========================================\n");
    printf(" ALL DHT11 ADAPTER TESTS PASSED\n");
    printf("========================================\n");


    return 0;
}
