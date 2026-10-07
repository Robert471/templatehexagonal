#include "dht11_sil_model.h"

#include <string.h>


#define DHT11_SIL_SEQUENCE_SIZE    256U


typedef struct
{
    Dht11_PinState sequence[DHT11_SIL_SEQUENCE_SIZE];

    uint16_t sequence_length;

    uint16_t sequence_index;

    uint8_t humidity;

    uint8_t temperature;

    uint32_t read_calls;

    uint32_t delay_calls;

    uint32_t set_mode_calls;

    uint32_t write_calls;

} Dht11_Sil_Model;


static Dht11_Sil_Model model;


/* ============================================================
 * Agregar estado lógico a la secuencia
 * ============================================================ */

static void append_state(
    Dht11_PinState state
)
{
    if (model.sequence_length >=
        DHT11_SIL_SEQUENCE_SIZE)
    {
        return;
    }

    model.sequence[
        model.sequence_length
    ] = state;

    model.sequence_length++;
}


/* ============================================================
 * Agregar un bit DHT11
 *
 * El Adapter realiza:
 *
 * WAIT HIGH
 * delay 30 us
 * READ
 * WAIT LOW
 *
 * Por eso el modelo produce:
 *
 * HIGH -> valor -> LOW
 *
 * ============================================================ */

static void append_bit(
    uint8_t value
)
{
    append_state(
        DHT11_PIN_HIGH
    );


    if (value != 0U)
    {
        append_state(
            DHT11_PIN_HIGH
        );
    }
    else
    {
        append_state(
            DHT11_PIN_LOW
        );
    }


    append_state(
        DHT11_PIN_LOW
    );
}


/* ============================================================
 * Agregar byte MSB first
 * ============================================================ */

static void append_byte(
    uint8_t value
)
{
    uint8_t bit;

    for (bit = 8U; bit > 0U; bit--)
    {
        append_bit(
            (uint8_t)(
                (value >> (bit - 1U)) & 0x01U
            )
        );
    }
}


/* ============================================================
 * Construir respuesta DHT11
 * ============================================================ */

static void build_response(void)
{
    uint8_t checksum;


    model.sequence_length = 0U;

    model.sequence_index = 0U;


    /*
     * Respuesta inicial del sensor:
     *
     * LOW
     * HIGH
     * LOW
     */

    append_state(
        DHT11_PIN_LOW
    );

    append_state(
        DHT11_PIN_HIGH
    );

    append_state(
        DHT11_PIN_LOW
    );


    /*
     * Checksum DHT11:
     *
     * H + Hdec + T + Tdec
     */

    checksum =
        (uint8_t)(
            model.humidity +
            model.temperature
        );


    /*
     * 40 bits.
     */

    append_byte(
        model.humidity
    );

    append_byte(
        0U
    );

    append_byte(
        model.temperature
    );

    append_byte(
        0U
    );

    append_byte(
        checksum
    );
}


/* ============================================================
 * Callback: set mode
 * ============================================================ */

static void sil_set_mode(
    void *context,
    Dht11_PinMode mode
)
{
    (void)context;
    (void)mode;

    model.set_mode_calls++;
}


/* ============================================================
 * Callback: write
 * ============================================================ */

static void sil_write(
    void *context,
    Dht11_PinState state
)
{
    (void)context;
    (void)state;

    model.write_calls++;
}


/* ============================================================
 * Callback: read
 * ============================================================ */

static Dht11_PinState sil_read(
    void *context
)
{
    (void)context;

    model.read_calls++;


    if (model.sequence_index <
        model.sequence_length)
    {
        return model.sequence[
            model.sequence_index++
        ];
    }


    /*
     * Si se agotó la trama,
     * dejamos el bus en LOW.
     */

    return DHT11_PIN_LOW;
}


/* ============================================================
 * Callback: delay
 * ============================================================ */

static void sil_delay_us(
    void *context,
    uint32_t microseconds
)
{
    (void)context;
    (void)microseconds;

    model.delay_calls++;
}


/* ============================================================
 * Reset
 * ============================================================ */

void dht11_sil_model_reset(void)
{
    memset(
        &model,
        0,
        sizeof(model)
    );


    /*
     * Valor inicial.
     */

    model.humidity = 50U;

    model.temperature = 25U;


    build_response();
}


/* ============================================================
 * Configurar nueva muestra
 * ============================================================ */

void dht11_sil_model_set_sample(
    uint8_t humidity,
    uint8_t temperature
)
{
    model.humidity =
        humidity;

    model.temperature =
        temperature;


    build_response();
}


/* ============================================================
 * Obtener Dht11_Port
 * ============================================================ */

const Dht11_Port *
dht11_sil_model_get_port(void)
{
    static Dht11_Port port;


    port.gpio_context =
        &model;

    port.timer_context =
        &model;

    port.set_mode =
        sil_set_mode;

    port.write =
        sil_write;

    port.read =
        sil_read;

    port.delay_us =
        sil_delay_us;


    return &port;
}


/* ============================================================
 * Estadísticas
 * ============================================================ */

uint32_t
dht11_sil_model_get_read_calls(void)
{
    return model.read_calls;
}


uint32_t
dht11_sil_model_get_delay_calls(void)
{
    return model.delay_calls;
}


uint32_t
dht11_sil_model_get_set_mode_calls(void)
{
    return model.set_mode_calls;
}


uint32_t
dht11_sil_model_get_write_calls(void)
{
    return model.write_calls;
}
