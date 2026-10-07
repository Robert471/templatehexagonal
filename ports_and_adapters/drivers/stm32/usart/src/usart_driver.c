#include "../inc/usart_driver.h"
#include <stddef.h>

/*
 * Inicializa el driver UART.
 *
 * El driver recibe explícitamente el handle de HAL mediante
 * inyección de dependencias.
 *
 * Esto evita depender de variables globales como:
 *
 *     extern UART_HandleTypeDef huart2;
 *
 * De esta manera, el driver puede utilizar cualquier UART
 * configurado por la capa de composición.
 */
void usart_driver_init(
    Usart_Driver *driver,
    UART_HandleTypeDef *handle,
    uint32_t timeout)
{
    if ((driver == NULL) || (handle == NULL))
    {
        return;
    }
    driver->handle = handle;
    driver->timeout = timeout;
}

/*
 * Implementación concreta del puerto Serial_Port.
 *
 * IMPORTANTE:
 *
 * Esta función pertenece a Infrastructure/Driver.
 * Por lo tanto, NO debe devolver CommStatus.
 *
 * CommStatus pertenece a Application.
 *
 * El driver solamente conoce:
 *
 *     HAL_StatusTypeDef
 *              ↓
 *        SerialStatus
 *
 * Luego Serial_Adapter será quien traduzca:
 *
 *     SerialStatus
 *              ↓
 *          CommStatus
 *
 * Esto mantiene la dirección de dependencias de la
 * arquitectura hexagonal.
 */
SerialStatus usart_driver_send(
    void *context,
   const uint8_t *data,
    uint16_t size)
{
    Usart_Driver *driver = (Usart_Driver *)context;
    HAL_StatusTypeDef hal_status;
    /*
     * Validación defensiva.
     *
     * El driver no debe intentar acceder al hardware
     * si no recibió una dependencia válida.
     */
    if ((driver == NULL) ||
        (driver->handle == NULL) ||
        (data == NULL) ||
        (size == 0U))
    {
        return SERIAL_ERROR_INVALID_PARAMETER;
    }

    /*
     * Llamada al hardware mediante STM32 HAL.
     *
     * Esta es precisamente la responsabilidad del Driver:
     * traducir la abstracción Serial_Port hacia el hardware.
     */
    /*
     * MISRA deviation at HAL boundary:
     * HAL_UART_Transmit() declares its buffer as non-const although
     * transmission does not modify it. The application-facing port
     * remains const-correct; only this STM32 HAL boundary removes const.
     */
    hal_status = HAL_UART_Transmit(
        driver->handle,
        (uint8_t *)data,
        size,
        driver->timeout
    );
    /*
     * Traducimos el resultado específico de STM32 HAL
     * a nuestro resultado abstracto del puerto.
     *
     * La capa superior no necesita saber qué HAL utilizamos.
     */
    if (hal_status == HAL_OK)
    {
        return SERIAL_OK;
    }
    return SERIAL_ERROR_TRANSMIT;
}