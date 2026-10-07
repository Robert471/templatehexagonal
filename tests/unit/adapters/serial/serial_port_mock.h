#ifndef SERIAL_PORT_MOCK_H
#define SERIAL_PORT_MOCK_H

#include <stdint.h>

#include "../../../../ports_and_adapters/ports/hardware/serial/serial_port.h"


typedef struct
{
    uint8_t buffer[128];
    uint16_t size;

    uint32_t send_call_count;

    Serial_Port port;

} Serial_Port_Mock;


void serial_port_mock_init(
    Serial_Port_Mock *mock
);


const Serial_Port *serial_port_mock_get_port(
    Serial_Port_Mock *mock
);


#endif /* SERIAL_PORT_MOCK_H */
