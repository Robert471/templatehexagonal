#ifndef SERIAL_SIL_MODEL_H
#define SERIAL_SIL_MODEL_H

#include <stdint.h>

#include "../../../ports_and_adapters/ports/hardware/serial/serial_port.h"


#define SERIAL_SIL_BUFFER_SIZE    128U


/*
 * Reinicia el UART virtual.
 */
void serial_sil_model_reset(void);


/*
 * Obtiene el Serial_Port virtual.
 */
const Serial_Port *
serial_sil_model_get_port(void);


/*
 * Últimos datos transmitidos.
 */
const uint8_t *
serial_sil_model_get_data(void);


uint16_t
serial_sil_model_get_length(void);


/*
 * Número de transmisiones.
 */
uint32_t
serial_sil_model_get_send_calls(void);

#endif /* SERIAL_SIL_MODEL_H */
