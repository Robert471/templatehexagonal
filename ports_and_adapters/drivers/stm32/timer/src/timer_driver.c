#include "../inc/timer_driver.h"

#include <stddef.h>

/*
 * Timer Driver
 * ------------
 * This driver is intentionally thin: TIM3 is configured and started by
 * the STM32/CubeMX application layer, while this component only exposes
 * the microsecond operation required by the DHT11 port.
 *
 * The original TIM3 timing implementation is preserved exactly so the
 * existing SIL/HIL behavior is not changed by the SOLID refactor.
 */
void timer_driver_init(
    Timer_Driver *driver,
    TIM_HandleTypeDef *handle)
{
    if ((driver == NULL) || (handle == NULL))
    {
        return;
    }

    driver->handle = handle;
}

void timer_driver_delay_us(
    void *context,
    uint32_t us)
{
    Timer_Driver *driver = (Timer_Driver *)context;

    if ((driver == NULL) || (driver->handle == NULL))
    {
        return;
    }

    /*
     * TIM3 is the injected timing dependency.
     * No global htim3 is referenced here.
     */
    __HAL_TIM_SET_COUNTER(driver->handle, 0U);

    while (__HAL_TIM_GET_COUNTER(driver->handle) < us)
    {
        /*
         * Busy wait is intentional: DHT11 requires microsecond-level
         * timing and this operation is isolated inside the driver.
         */
    }
}
