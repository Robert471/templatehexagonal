#ifndef TEMPERATURE_DOMAIN_H
#define TEMPERATURE_DOMAIN_H

#include <stdint.h>


/* ============================================================
 * Datos del sensor
 * ============================================================ */

typedef struct
{
    uint8_t temperature;
    uint8_t humidity;

} TemperatureData;


/* ============================================================
 * Nivel de temperatura
 * ============================================================ */

typedef enum
{
    TEMPERATURE_LEVEL_LOW = 0,
    TEMPERATURE_LEVEL_NORMAL,
    TEMPERATURE_LEVEL_HIGH

} TemperatureLevel;


/* ============================================================
 * Reglas de negocio
 * ============================================================ */

TemperatureLevel temperature_classify(
    uint8_t temperature
);


#endif /* TEMPERATURE_DOMAIN_H */