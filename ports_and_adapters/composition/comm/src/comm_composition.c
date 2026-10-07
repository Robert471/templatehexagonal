#include <stddef.h>

#include "../inc/comm_composition.h"

#include "../../../ports/application/comm/comm_port.h"
#include "../../../ports/hardware/serial/serial_port.h"
#include "../../../adapters/serial/inc/serial_adapter.h"
#include "../../../drivers/stm32/usart/inc/usart_driver.h"

/*
 * Composition Root de comunicación.
 *
 * La única capa que conoce simultáneamente Application, Adapter, Driver y
 * HAL es esta composición. El resto de la arquitectura depende de contratos.
 */
static Usart_Driver usart_driver;
static Serial_Port serial_port;
static Serial_Adapter serial_adapter;
static Comm_Port comm_port;

void comm_composition_init(
    Comm_Application *application,
    UART_HandleTypeDef *uart_handle)
{
    if ((application == NULL) || (uart_handle == NULL))
    {
        return;
    }

    usart_driver_init(
        &usart_driver,
        uart_handle,
        HAL_MAX_DELAY
    );

    serial_port.context = &usart_driver;
    serial_port.send = usart_driver_send;

    serial_adapter_init(
        &serial_adapter,
        &serial_port
    );

    comm_port.context = &serial_adapter;
    comm_port.send_temperature = serial_adapter_send_temperature;

    comm_application_init(
        application,
        &comm_port
    );
}
