#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


/* ============================================================
 * DHT11 Port
 * ============================================================ */

#include "../../../ports_and_adapters/ports/hardware/dht11/dht11_port.h"


/* ============================================================
 * Serial Port
 * ============================================================ */

#include "../../../ports_and_adapters/ports/hardware/serial/serial_port.h"


/* ============================================================
 * Sensor Application Port
 * ============================================================ */

#include "../../../ports_and_adapters/ports/application/sensor/sensor_port.h"


/* ============================================================
 * Communication Application Port
 * ============================================================ */

#include "../../../ports_and_adapters/ports/application/comm/comm_port.h"


/* ============================================================
 * Domain
 * ============================================================ */

#include "../../../ports_and_adapters/domain/inc/temperature_domain.h"


/* ============================================================
 * Applications
 * ============================================================ */

#include "../../../ports_and_adapters/application/inc/sensor_application.h"

#include "../../../ports_and_adapters/application/inc/comm_application.h"


/* ============================================================
 * DHT11 Adapter
 * ============================================================ */

#include "../../../ports_and_adapters/adapters/dht11/inc/dht11_adapter.h"


/* ============================================================
 * Serial Adapter
 * ============================================================ */

#include "../../../ports_and_adapters/adapters/serial/inc/serial_adapter.h"


/* ============================================================
 * DHT11 Integration Mock
 * ============================================================ */

void dht11_integration_mock_init(void);

const Dht11_Port *
dht11_integration_mock_get_port(void);

uint32_t
dht11_integration_mock_get_set_mode_calls(void);

uint32_t
dht11_integration_mock_get_write_calls(void);

uint32_t
dht11_integration_mock_get_read_calls(void);

uint32_t
dht11_integration_mock_get_delay_calls(void);


/* ============================================================
 * Serial Integration Mock
 * ============================================================ */

typedef struct
{
    uint8_t buffer[64];

    uint16_t size;

    uint32_t send_call_count;

} Serial_Integration_Mock;


static Serial_Integration_Mock serial_mock;


/* ============================================================
 * Serial Mock Init
 * ============================================================ */

static void serial_integration_mock_init(void)
{
    memset(
        &serial_mock,
        0,
        sizeof(serial_mock)
    );
}


/* ============================================================
 * Serial Port callback
 * ============================================================ */

static SerialStatus serial_mock_send(
    void *context,
    const uint8_t *data,
    uint16_t length
)
{
    Serial_Integration_Mock *mock =
        (Serial_Integration_Mock *)context;


    assert(
        mock != NULL
    );


    assert(
        data != NULL
    );


    assert(
        length < sizeof(mock->buffer)
    );


    memcpy(
        mock->buffer,
        data,
        length
    );


    mock->buffer[length] = '\0';

    mock->size = length;

    mock->send_call_count++;

    return SERIAL_OK;
}


/* ============================================================
 * Test
 *
 * Flujo:
 *
 * DHT11 Mock
 *     ↓
 * Dht11_Port
 *     ↓
 * Dht11_Adapter
 *     ↓
 * Sensor_Port
 *     ↓
 * Sensor_Application
 *     ↓
 * TemperatureData
 *     ↓
 * Comm_Application
 *     ↓
 * Comm_Port
 *     ↓
 * Serial_Adapter
 *     ↓
 * Serial_Port
 *     ↓
 * Serial Mock
 * ============================================================ */

static void test_sensor_to_serial(void)
{
    Dht11_Adapter dht11_adapter;

    Sensor_Port sensor_port;

    Sensor_Application sensor_application;

    Serial_Integration_Mock *serial_mock_ptr;

    Serial_Port serial_port;

    Serial_Adapter serial_adapter;

    Comm_Port comm_port;

    Comm_Application comm_application;

    TemperatureData data;

    TemperatureLevel temperature_level;

    SensorStatus sensor_status;

    CommStatus comm_status;


    /* ========================================================
     * 1. Inicializar DHT11 Mock
     * ======================================================== */

    dht11_integration_mock_init();


    /* ========================================================
     * 2. Inicializar DHT11 Adapter
     *
     * DHT11 Mock
     *     ↓
     * Dht11_Port
     *     ↓
     * Dht11_Adapter
     * ======================================================== */

    dht11_adapter_init(
        &dht11_adapter,
        dht11_integration_mock_get_port()
    );


    /* ========================================================
     * 3. Construir Sensor_Port
     *
     * Sensor_Application
     *       ↓
     * Sensor_Port
     *       ↓
     * Dht11_Adapter
     * ======================================================== */

    sensor_port.context =
        &dht11_adapter;

    sensor_port.read =
        dht11_adapter_read;


    /* ========================================================
     * 4. Inicializar Sensor Application
     * ======================================================== */

    sensor_application_init(
        &sensor_application,
        &sensor_port
    );


    /* ========================================================
     * 5. Inicializar Serial Mock
     * ======================================================== */

    serial_integration_mock_init();


    serial_mock_ptr =
        &serial_mock;


    /* ========================================================
     * 6. Construir Serial_Port
     *
     * Serial_Adapter
     *       ↓
     * Serial_Port
     *       ↓
     * Serial Mock
     * ======================================================== */

    serial_port.context =
        serial_mock_ptr;

    serial_port.send =
        serial_mock_send;


    /* ========================================================
     * 7. Inicializar Serial Adapter
     * ======================================================== */

    serial_adapter_init(
        &serial_adapter,
        &serial_port
    );


    /* ========================================================
     * 8. Construir Comm_Port
     *
     * Comm_Application
     *       ↓
     * Comm_Port
     *       ↓
     * Serial_Adapter
     * ======================================================== */

    comm_port.context =
        &serial_adapter;

    comm_port.send_temperature =
        serial_adapter_send_temperature;


    /* ========================================================
     * 9. Inicializar Comm Application
     * ======================================================== */

    comm_application_init(
        &comm_application,
        &comm_port
    );


    /* ========================================================
     * 10. Leer sensor
     *
     * DHT11
     *   ↓
     * DHT11 Adapter
     *   ↓
     * Sensor_Port
     *   ↓
     * Sensor_Application
     * ======================================================== */

    memset(
        &data,
        0,
        sizeof(data)
    );


    sensor_status =
        sensor_application_read(
            &sensor_application,
            &data,
            &temperature_level
        );


    /* ========================================================
     * 11. Verificar lectura
     * ======================================================== */

    assert(
        sensor_status ==
        SENSOR_OK
    );


    assert(
        data.temperature == 28U
    );


    assert(
        data.humidity == 65U
    );


    /* ========================================================
     * 12. Verificar Domain
     * ======================================================== */

    assert(
        temperature_level ==
        TEMPERATURE_LEVEL_NORMAL
    );


    /* ========================================================
     * 13. Verificar interacción DHT11
     * ======================================================== */

    assert(
        dht11_integration_mock_get_set_mode_calls() >= 2U
    );


    assert(
        dht11_integration_mock_get_write_calls() >= 2U
    );


    assert(
        dht11_integration_mock_get_read_calls() > 0U
    );


    assert(
        dht11_integration_mock_get_delay_calls() > 0U
    );


    /* ========================================================
     * 14. Enviar temperatura
     *
     * TemperatureData
     *       ↓
     * Comm_Application
     *       ↓
     * Comm_Port
     *       ↓
     * Serial_Adapter
     *       ↓
     * Serial_Port
     *       ↓
     * Serial Mock
     * ======================================================== */

    comm_status =
        comm_application_send_temperature(
            &comm_application,
            &data
        );


    /* ========================================================
     * 15. Verificar Comm
     * ======================================================== */

    assert(
        comm_status ==
        COMM_OK
    );


    /* ========================================================
     * 16. Verificar Serial
     * ======================================================== */

    assert(
        serial_mock.send_call_count == 1U
    );


    assert(
        serial_mock.size ==
        strlen(
            "Temperature: 28 C\r\n"
        )
    );


    assert(
        memcmp(
            serial_mock.buffer,
            "Temperature: 28 C\r\n",
            serial_mock.size
        ) == 0
    );


    /* ========================================================
     * Resultado
     * ======================================================== */

    printf(
        "[PASS] sensor_to_serial\n"
    );

    printf(
        "       Temperature = %u C\n",
        data.temperature
    );

    printf(
        "       Humidity    = %u %%\n",
        data.humidity
    );

    printf(
        "       Level       = NORMAL\n"
    );

    printf(
        "       Serial      = %s",
        serial_mock.buffer
    );
}


/* ============================================================
 * MAIN
 * ============================================================ */

int main(void)
{
    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        " SENSOR -> SERIAL INTEGRATION TEST\n"
    );

    printf(
        "========================================\n\n"
    );


    test_sensor_to_serial();


    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        " INTEGRATION TEST PASSED\n"
    );

    printf(
        "========================================\n"
    );


    return 0;
}