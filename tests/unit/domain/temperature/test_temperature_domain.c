#include <assert.h>
#include <stdio.h>

#include "../../../../ports_and_adapters/domain/inc/temperature_domain.h"


/*
 * ============================================================
 * LOW
 * ============================================================
 */

static void test_temperature_0_is_low(void)
{
    TemperatureLevel result;

    result = temperature_classify(0U);

    assert(result == TEMPERATURE_LEVEL_LOW);
}


static void test_temperature_20_is_low(void)
{
    TemperatureLevel result;

    result = temperature_classify(20U);

    assert(result == TEMPERATURE_LEVEL_LOW);
}


/*
 * ============================================================
 * NORMAL
 * ============================================================
 */

static void test_temperature_21_is_normal(void)
{
    TemperatureLevel result;

    result = temperature_classify(21U);

    assert(result == TEMPERATURE_LEVEL_NORMAL);
}


static void test_temperature_30_is_normal(void)
{
    TemperatureLevel result;

    result = temperature_classify(30U);

    assert(result == TEMPERATURE_LEVEL_NORMAL);
}


/*
 * ============================================================
 * HIGH
 * ============================================================
 */

static void test_temperature_31_is_high(void)
{
    TemperatureLevel result;

    result = temperature_classify(31U);

    assert(result == TEMPERATURE_LEVEL_HIGH);
}


static void test_temperature_255_is_high(void)
{
    TemperatureLevel result;

    result = temperature_classify(255U);

    assert(result == TEMPERATURE_LEVEL_HIGH);
}


/*
 * ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    printf("Running Temperature Domain tests...\n");

    test_temperature_0_is_low();
    test_temperature_20_is_low();

    test_temperature_21_is_normal();
    test_temperature_30_is_normal();

    test_temperature_31_is_high();
    test_temperature_255_is_high();

    printf("Temperature Domain tests: PASS\n");

    return 0;
}
