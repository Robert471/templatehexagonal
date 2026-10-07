#ifndef COMM_APPLICATION_H
#define COMM_APPLICATION_H

#include "../../domain/inc/temperature_domain.h"
#include "../../ports/application/comm/comm_port.h"


typedef struct
{
    Comm_Port comm_port;

} Comm_Application;


void comm_application_init(
    Comm_Application *application,
    const Comm_Port *comm_port
);


CommStatus comm_application_send_temperature(
    Comm_Application *application,
    const TemperatureData *data
);


#endif /* COMM_APPLICATION_H */
