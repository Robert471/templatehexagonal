#ifndef DHT11_ADAPTER_H
#define DHT11_ADAPTER_H

#include "../../../ports/application/sensor/sensor_port.h"
#include "../../../ports/hardware/dht11/dht11_port.h"

typedef struct
{
    Dht11_Port port;
} Dht11_Adapter;

/*
 * Inicializa el adapter con su dependencia
 * de hardware.
 */
void dht11_adapter_init(Dht11_Adapter *adapter, const Dht11_Port *port);

/*
 * Función que implementa Sensor_Port.
 */
SensorStatus dht11_adapter_read(void *context, TemperatureData *data);

#endif /* DHT11_ADAPTER_H */
