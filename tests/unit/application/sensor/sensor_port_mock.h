#ifndef SENSOR_PORT_MOCK_H
#define SENSOR_PORT_MOCK_H

#include <stdint.h>

#include "../../../../ports_and_adapters/ports/application/sensor/sensor_port.h"

typedef struct
{
    SensorStatus expected_status;
    TemperatureData expected_data;
    uint32_t read_call_count;

} Sensor_Port_Mock;

void sensor_port_mock_init(
    Sensor_Port_Mock *mock
);

SensorStatus sensor_port_mock_read(
    void *context,
    TemperatureData *data
);

const Sensor_Port *sensor_port_mock_get_port(
    Sensor_Port_Mock *mock
);

#endif