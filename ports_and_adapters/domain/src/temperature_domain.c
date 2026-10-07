
#include "../inc/temperature_domain.h"


/* ============================================================
 * Reglas de negocio de temperatura
 * ============================================================ */

#define TEMPERATURE_LOW_MAX      20U
#define TEMPERATURE_NORMAL_MAX   30U


TemperatureLevel temperature_classify(
    uint8_t temperature)
{
    if (temperature <= TEMPERATURE_LOW_MAX)
    {
        return TEMPERATURE_LEVEL_LOW;
    }

    if (temperature <= TEMPERATURE_NORMAL_MAX)
    {
        return TEMPERATURE_LEVEL_NORMAL;
    }

    return TEMPERATURE_LEVEL_HIGH;
}
