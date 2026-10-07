#ifndef SERIAL_ADAPTER_H
#define SERIAL_ADAPTER_H

#include <stdint.h>

#include "../../../ports/application/comm/comm_port.h"
#include "../../../ports/hardware/serial/serial_port.h"
#include "../../../domain/inc/temperature_domain.h"


typedef struct
{
    Serial_Port serial_port;

} Serial_Adapter;


/*
 * Inicializa el Adapter con su Hardware Port.
 */
void serial_adapter_init(
    Serial_Adapter *adapter,
    const Serial_Port *serial_port
);


/*
 * Implementación del Comm_Port.
 */
CommStatus serial_adapter_send_temperature(
    void *context,
    const TemperatureData *data
);

#endif /* SERIAL_ADAPTER_H */
