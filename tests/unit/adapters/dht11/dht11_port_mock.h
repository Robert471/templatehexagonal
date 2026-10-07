#ifndef DHT11_PORT_MOCK_H
#define DHT11_PORT_MOCK_H

#include <stdint.h>

#include "../../../../ports_and_adapters/ports/hardware/dht11/dht11_port.h"


typedef struct
{
    Dht11_PinMode mode;
    Dht11_PinState state;

    Dht11_PinState sequence[128];

    uint16_t sequence_length;
    uint16_t sequence_index;

    uint32_t set_mode_call_count;
    uint32_t write_call_count;
    uint32_t read_call_count;
    uint32_t delay_call_count;

    uint32_t last_delay_us;

    Dht11_Port port;

} Dht11_Port_Mock;


void dht11_port_mock_init(
    Dht11_Port_Mock *mock
);


void dht11_port_mock_set_sequence(
    Dht11_Port_Mock *mock,
    const Dht11_PinState *sequence,
    uint16_t length
);


const Dht11_Port *dht11_port_mock_get_port(
    Dht11_Port_Mock *mock
);


#endif /* DHT11_PORT_MOCK_H */
