#ifndef DHT11_SIL_MODEL_H
#define DHT11_SIL_MODEL_H

#include <stdint.h>

#include "../../../ports_and_adapters/ports/hardware/dht11/dht11_port.h"


/*
 * Reinicia el modelo SIL.
 */
void dht11_sil_model_reset(void);


/*
 * Configura la muestra que el DHT11 virtual
 * entregará en la siguiente lectura.
 *
 * La humedad forma parte de la trama DHT11,
 * aunque actualmente la aplicación solamente
 * expone la temperatura en TemperatureData.
 */
void dht11_sil_model_set_sample(
    uint8_t humidity,
    uint8_t temperature
);


/*
 * Obtiene el Hardware Port virtual.
 */
const Dht11_Port *
dht11_sil_model_get_port(void);


/*
 * Estadísticas de ejecución.
 */
uint32_t
dht11_sil_model_get_read_calls(void);

uint32_t
dht11_sil_model_get_delay_calls(void);

uint32_t
dht11_sil_model_get_set_mode_calls(void);

uint32_t
dht11_sil_model_get_write_calls(void);

#endif /* DHT11_SIL_MODEL_H */
