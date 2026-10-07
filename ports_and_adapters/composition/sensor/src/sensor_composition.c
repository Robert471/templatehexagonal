#include <stddef.h>
#include <stdint.h>

#include "../inc/sensor_composition.h"

#include "../../../ports/application/sensor/sensor_port.h"
#include "../../../ports/hardware/dht11/dht11_port.h"

#include "../../../adapters/dht11/inc/dht11_adapter.h"

#include "../../../drivers/stm32/gpio/inc/gpio_driver.h"
#include "../../../drivers/stm32/timer/inc/timer_driver.h"

#include "stm32f4xx_hal.h"

#define DHT11_GPIO_PORT GPIOC
#define DHT11_GPIO_PIN  GPIO_PIN_3

/*
 * Composition Root del subsistema de sensor.
 *
 * Aquí sí está permitido conocer STM32/HAL: este archivo existe precisamente
 * para ensamblar infraestructura concreta con los puertos hexagonales.
 * Application y Domain permanecen completamente ajenos al hardware.
 */
static Gpio_Driver      gpio_driver;
static Timer_Driver     timer_driver;
static Dht11_Adapter    dht11_adapter;
static Dht11_Port       dht11_port;
static Sensor_Port      sensor_port;

void sensor_composition_init(
    Sensor_Application *application,
    TIM_HandleTypeDef *timer_handle)
{
    if ((application == NULL) || (timer_handle == NULL))
    {
        return;
    }

    gpio_driver_init(
        &gpio_driver,
        DHT11_GPIO_PORT,
        DHT11_GPIO_PIN
    );

    timer_driver_init(
        &timer_driver,
        timer_handle
    );

    /* Dht11_Port queda expresado sólo en términos de su contrato. */
    /*
     * Cada operación recibe el contexto del driver que realmente
     * implementa esa operación. No podemos compartir un único
     * context porque GPIO y TIM3 son dos dependencias distintas.
     */
    dht11_port.gpio_context  = &gpio_driver;
    dht11_port.timer_context = &timer_driver;
    dht11_port.set_mode      = gpio_driver_set_mode;
    dht11_port.write         = gpio_driver_write;
    dht11_port.read          = gpio_driver_read;
    dht11_port.delay_us      = timer_driver_delay_us;

    /* Adapter = lógica de protocolo + Port de hardware. */
    dht11_adapter_init(
        &dht11_adapter,
        &dht11_port
    );

    /* Port de aplicación = frontera entre Application e infraestructura. */
    sensor_port.context = &dht11_adapter;
    sensor_port.read    = dht11_adapter_read;

    sensor_application_init(
        application,
        &sensor_port
    );
}
