#include "../inc/gpio_driver.h"

#include <stddef.h>

/*
 * The GPIO driver is the only layer that knows STM32 GPIO details.
 * No Application or Adapter code depends directly on HAL_GPIO_*.
 */
static void gpio_driver_enable_clock(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#ifdef GPIOE
    else if (port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif
#ifdef GPIOF
    else if (port == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#ifdef GPIOG
    else if (port == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
#ifdef GPIOH
    else if (port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif
}

void gpio_driver_init(
    Gpio_Driver *driver,
    GPIO_TypeDef *port,
    uint16_t pin)
{
    if ((driver == NULL) || (port == NULL))
    {
        return;
    }

    gpio_driver_enable_clock(port);

    driver->port = port;
    driver->pin = pin;
}

void gpio_driver_set_mode(
    void *context,
    Dht11_PinMode mode)
{
    Gpio_Driver *driver = (Gpio_Driver *)context;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if ((driver == NULL) || (driver->port == NULL))
    {
        return;
    }

    GPIO_InitStruct.Pin = driver->pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    if (mode == DHT11_PIN_OUTPUT)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    }
    else
    {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    }

    HAL_GPIO_Init(driver->port, &GPIO_InitStruct);
}

void gpio_driver_write(
    void *context,
    Dht11_PinState state)
{
    Gpio_Driver *driver = (Gpio_Driver *)context;

    if ((driver == NULL) || (driver->port == NULL))
    {
        return;
    }

    HAL_GPIO_WritePin(
        driver->port,
        driver->pin,
        (state == DHT11_PIN_HIGH)
            ? GPIO_PIN_SET
            : GPIO_PIN_RESET
    );
}

Dht11_PinState gpio_driver_read(void *context)
{
    Gpio_Driver *driver = (Gpio_Driver *)context;

    if ((driver == NULL) || (driver->port == NULL))
    {
        return DHT11_PIN_LOW;
    }

    return (HAL_GPIO_ReadPin(
                driver->port,
                driver->pin) == GPIO_PIN_SET)
        ? DHT11_PIN_HIGH
        : DHT11_PIN_LOW;
}
