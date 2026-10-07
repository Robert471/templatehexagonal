#include "serial_sil_model.h"

#include <string.h>


typedef struct
{
    uint8_t buffer[
        SERIAL_SIL_BUFFER_SIZE
    ];

    uint16_t length;

    uint32_t send_calls;

} Serial_Sil_Model;


static Serial_Sil_Model model;


/* ============================================================
 * UART virtual
 * ============================================================ */

static SerialStatus sil_send(
    void *context,
    const uint8_t *data,
    uint16_t length
)
{
    Serial_Sil_Model *serial =
        (Serial_Sil_Model *)context;


    if (serial == NULL)
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }


    if (data == NULL)
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }


    if (length >=
        SERIAL_SIL_BUFFER_SIZE)
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }


    memcpy(
        serial->buffer,
        data,
        length
    );


    serial->buffer[length] =
        '\0';


    serial->length =
        length;

    serial->send_calls++;

    return SERIAL_OK;
}


/* ============================================================
 * Reset
 * ============================================================ */

void serial_sil_model_reset(void)
{
    memset(
        &model,
        0,
        sizeof(model)
    );
}


/* ============================================================
 * Obtener Serial_Port
 * ============================================================ */

const Serial_Port *
serial_sil_model_get_port(void)
{
    static Serial_Port port;


    port.context =
        &model;

    port.send =
        sil_send;


    return &port;
}


/* ============================================================
 * Obtener datos
 * ============================================================ */

const uint8_t *
serial_sil_model_get_data(void)
{
    return model.buffer;
}


uint16_t
serial_sil_model_get_length(void)
{
    return model.length;
}


uint32_t
serial_sil_model_get_send_calls(void)
{
    return model.send_calls;
}
