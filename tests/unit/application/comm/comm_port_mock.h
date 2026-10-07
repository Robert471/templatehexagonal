#ifndef COMM_PORT_MOCK_H
#define COMM_PORT_MOCK_H

#include <stdint.h>

#include "../../../../ports_and_adapters/ports/application/comm/comm_port.h"

typedef struct
{
    CommStatus expected_status;
    TemperatureData last_data;

    uint32_t send_temperature_call_count;

} Comm_Port_Mock;


void comm_port_mock_init(
    Comm_Port_Mock *mock
);


CommStatus comm_port_mock_send_temperature(
    void *context,
    const TemperatureData *data
);


const Comm_Port *comm_port_mock_get_port(
    Comm_Port_Mock *mock
);

#endif