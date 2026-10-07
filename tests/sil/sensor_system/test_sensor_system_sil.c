#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


/* ============================================================
 * Domain
 * ============================================================ */

#include "../../../ports_and_adapters/domain/inc/temperature_domain.h"


/* ============================================================
 * Ports
 * ============================================================ */

#include "../../../ports_and_adapters/ports/application/sensor/sensor_port.h"

#include "../../../ports_and_adapters/ports/application/comm/comm_port.h"


/* ============================================================
 * Applications
 * ============================================================ */

#include "../../../ports_and_adapters/application/inc/sensor_application.h"

#include "../../../ports_and_adapters/application/inc/comm_application.h"


/* ============================================================
 * Adapters
 * ============================================================ */

#include "../../../ports_and_adapters/adapters/dht11/inc/dht11_adapter.h"

#include "../../../ports_and_adapters/adapters/serial/inc/serial_adapter.h"


/* ============================================================
 * SIL Models
 * ============================================================ */

#include "dht11_sil_model.h"

#include "serial_sil_model.h"


/* ============================================================
 * Obtener nivel esperado
 *
 * Reglas actuales del Domain:
 *
 * <= 20  -> LOW
 * <= 30  -> NORMAL
 * > 30   -> HIGH
 * ============================================================ */

static TemperatureLevel expected_level(
    uint8_t temperature
)
{
    if (temperature <= 20U)
    {
        return TEMPERATURE_LEVEL_LOW;
    }


    if (temperature <= 30U)
    {
        return TEMPERATURE_LEVEL_NORMAL;
    }


    return TEMPERATURE_LEVEL_HIGH;
}


/* ============================================================
 * Ejecutar una muestra SIL
 * ============================================================ */

static void run_sample(
    Sensor_Application *sensor_application,
    Comm_Application *comm_application,
    uint8_t humidity,
    uint8_t temperature,
    uint32_t sample_number
)
{
    TemperatureData data;

    TemperatureLevel level;

    TemperatureLevel expected;

    SensorStatus sensor_status;

    CommStatus comm_status;

    const uint8_t *serial_data;

    char expected_serial[64];


    /*
     * --------------------------------------------------------
     * Configurar sensor virtual
     * --------------------------------------------------------
     */

    dht11_sil_model_set_sample(
        humidity,
        temperature
    );


    /*
     * --------------------------------------------------------
     * Limpiar salida serial
     * --------------------------------------------------------
     */

    serial_sil_model_reset();


    /*
     * --------------------------------------------------------
     * Leer sensor
     * --------------------------------------------------------
     */

    memset(
        &data,
        0,
        sizeof(data)
    );


    level =
        TEMPERATURE_LEVEL_LOW;


    sensor_status =
        sensor_application_read(
            sensor_application,
            &data,
            &level
        );


    /*
     * --------------------------------------------------------
     * Verificar lectura
     * --------------------------------------------------------
     */

    assert(
        sensor_status ==
        SENSOR_OK
    );


    assert(
        data.temperature ==
        temperature
    );


    /*
     * --------------------------------------------------------
     * Verificar Domain
     * --------------------------------------------------------
     */

    expected =
        expected_level(
            temperature
        );


    assert(
        level ==
        expected
    );


    /*
     * --------------------------------------------------------
     * Enviar por Comm
     * --------------------------------------------------------
     */

    comm_status =
        comm_application_send_temperature(
            comm_application,
            &data
        );


    assert(
        comm_status ==
        COMM_OK
    );


    /*
     * --------------------------------------------------------
     * Verificar transmisión
     * --------------------------------------------------------
     */

    assert(
        serial_sil_model_get_send_calls() ==
        1U
    );


    serial_data =
        serial_sil_model_get_data();


    assert(
        serial_data != NULL
    );


    /*
     * El Serial Adapter actual
     * genera:
     *
     * Temperature: XX C\r\n
     */

    (void)snprintf(
        expected_serial,
        sizeof(expected_serial),
        "Temperature: %u C\r\n",
        (unsigned int)temperature
    );


    printf(
        "DEBUG expected=[%s] received=[%s] length=%u\\n",
        expected_serial,
        (const char *)serial_data,
        (unsigned int)serial_sil_model_get_length()
    );

    assert(
        strcmp(
            (const char *)serial_data,
            expected_serial
        ) == 0
    );


    /*
     * --------------------------------------------------------
     * Mostrar resultado
     * --------------------------------------------------------
     */

    printf(
        "[SIL %02lu] "
        "T=%02u C  "
        "H=%02u %%  ",
        (unsigned long)sample_number,
        (unsigned int)temperature,
        (unsigned int)humidity
    );


    if (level ==
        TEMPERATURE_LEVEL_LOW)
    {
        printf(
            "LEVEL=LOW  "
        );
    }
    else if (level ==
             TEMPERATURE_LEVEL_NORMAL)
    {
        printf(
            "LEVEL=NORMAL  "
        );
    }
    else
    {
        printf(
            "LEVEL=HIGH  "
        );
    }


    printf(
        "SERIAL=\"%s\"",
        serial_data
    );
}


/* ============================================================
 * MAIN
 * ============================================================ */

int main(void)
{
    Dht11_Adapter dht11_adapter;

    Sensor_Port sensor_port;

    Sensor_Application sensor_application;

    Serial_Adapter serial_adapter;

    Comm_Port comm_port;

    Comm_Application comm_application;


    uint32_t sample_number = 0U;


    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        " SENSOR SYSTEM SIL TEST\n"
    );

    printf(
        "========================================\n\n"
    );


    /* ========================================================
     * Inicializar modelos SIL
     * ======================================================== */

    dht11_sil_model_reset();

    serial_sil_model_reset();


    /* ========================================================
     * DHT11 Adapter
     * ======================================================== */

    dht11_adapter_init(
        &dht11_adapter,
        dht11_sil_model_get_port()
    );


    /* ========================================================
     * Sensor Port
     * ======================================================== */

    sensor_port.context =
        &dht11_adapter;

    sensor_port.read =
        dht11_adapter_read;


    /* ========================================================
     * Sensor Application
     * ======================================================== */

    sensor_application_init(
        &sensor_application,
        &sensor_port
    );


    /* ========================================================
     * Serial Adapter
     * ======================================================== */

    serial_adapter_init(
        &serial_adapter,
        serial_sil_model_get_port()
    );


    /* ========================================================
     * Comm Port
     * ======================================================== */

    comm_port.context =
        &serial_adapter;

    comm_port.send_temperature =
        serial_adapter_send_temperature;


    /* ========================================================
     * Comm Application
     * ======================================================== */

    comm_application_init(
        &comm_application,
        &comm_port
    );


    /* ========================================================
     * SECUENCIA ASCENDENTE
     *
     * 15 -> 35
     *
     * LOW
     * LOW
     * ...
     * NORMAL
     * ...
     * HIGH
     * ======================================================== */

    printf(
        "----------------------------------------\n"
    );

    printf(
        "ASCENDING TEMPERATURE\n"
    );

    printf(
        "----------------------------------------\n"
    );


    for (uint8_t temperature = 15U;
         temperature <= 35U;
         temperature++)
    {
        /*
         * Humedad variable:
         *
         * 50 -> 70 %
         */

        uint8_t humidity =
            (uint8_t)(
                50U +
                (temperature - 15U)
            );


        sample_number++;


        run_sample(
            &sensor_application,
            &comm_application,
            humidity,
            temperature,
            sample_number
        );
    }


    /* ========================================================
     * SECUENCIA DESCENDENTE
     *
     * 35 -> 15
     *
     * HIGH
     * ...
     * NORMAL
     * ...
     * LOW
     * ======================================================== */

    printf(
        "\n"
    );

    printf(
        "----------------------------------------\n"
    );

    printf(
        "DESCENDING TEMPERATURE\n"
    );

    printf(
        "----------------------------------------\n"
    );


    {
        uint8_t temperature = 35U;

        while (temperature >= 15U)
        {
            /*
             * Humedad variable:
             *
             * 60 -> 80 %
             */

            uint8_t humidity =
                (uint8_t)(
                    60U +
                    (35U - temperature)
                );

            sample_number++;

            run_sample(
                &sensor_application,
                &comm_application,
                humidity,
                temperature,
                sample_number
            );

            if (temperature == 15U)
            {
                break;
            }

            temperature--;
        }
    }


    /* ========================================================
     * Verificaciones globales
     * ======================================================== */

    assert(
        sample_number == 42U
    );


    assert(
        dht11_sil_model_get_read_calls() > 0U
    );


    assert(
        dht11_sil_model_get_delay_calls() > 0U
    );


    assert(
        dht11_sil_model_get_set_mode_calls() > 0U
    );


    assert(
        dht11_sil_model_get_write_calls() > 0U
    );


    /* ========================================================
     * Resultado
     * ======================================================== */

    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "SIL TEST PASSED\n"
    );

    printf(
        "Samples executed: %lu\n",
        (unsigned long)sample_number
    );

    printf(
        "========================================\n"
    );


    return 0;
}
