#ifndef COMM_COMPOSITION_H
#define COMM_COMPOSITION_H

#include "stm32f4xx_hal.h"
#include "../../../application/inc/comm_application.h"

/* Construye la cadena Comm Application -> Adapter -> Driver -> HAL. */
void comm_composition_init(
    Comm_Application *application,
    UART_HandleTypeDef *uart_handle
);

#endif /* COMM_COMPOSITION_H */
