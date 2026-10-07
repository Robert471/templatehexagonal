#include <stddef.h>

#include "../inc/comm_application.h"


void comm_application_init(
    Comm_Application *application,
    const Comm_Port *comm_port)
{
    if ((application == NULL) ||
        (comm_port == NULL))
    {
        return;
    }

    application->comm_port = *comm_port;
}


CommStatus comm_application_send_temperature(
    Comm_Application *application,
    const TemperatureData *data)
{
    if ((application == NULL) || (data == NULL))
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    if (application->comm_port.send_temperature == NULL)
    {
        return COMM_ERROR_INVALID_PARAMETER;
    }

    return application->comm_port.send_temperature(
        application->comm_port.context,
        data
    );
}
