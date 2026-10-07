cmake_minimum_required(VERSION 3.22)

project(sensorhumedad_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(PA_ROOT ${PROJECT_ROOT}/ports_and_adapters)

# =========================================================
# Compile-time quality checks for the test harness
# =========================================================
#
# These are not a substitute for a MISRA checker, but they
# catch the most common type/sign/implicit-conversion issues
# before the tests are accepted.
#
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
    )
endif()


# =========================================================
# TEMPERATURE DOMAIN
# =========================================================

add_executable(test_temperature_domain
    unit/domain/temperature/test_temperature_domain.c
    ${PA_ROOT}/domain/src/temperature_domain.c
)

target_include_directories(test_temperature_domain PRIVATE
    ${PA_ROOT}/domain/inc
)

add_test(
    NAME temperature_domain
    COMMAND test_temperature_domain
)


# =========================================================
# SENSOR APPLICATION
# =========================================================

add_executable(test_sensor_application
    unit/application/sensor/test_sensor_application.c
    unit/application/sensor/sensor_port_mock.c
    ${PA_ROOT}/application/src/sensor_application.c
    ${PA_ROOT}/domain/src/temperature_domain.c
)

target_include_directories(test_sensor_application PRIVATE
    ${PA_ROOT}/application/inc
    ${PA_ROOT}/domain/inc
    ${PA_ROOT}/ports/application/sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/application/sensor
)

add_test(
    NAME sensor_application
    COMMAND test_sensor_application
)


# =========================================================
# COMM APPLICATION
# =========================================================

add_executable(test_comm_application
    unit/application/comm/test_comm_application.c
    unit/application/comm/comm_port_mock.c
    ${PA_ROOT}/application/src/comm_application.c
)

target_include_directories(test_comm_application PRIVATE
    ${PA_ROOT}/application/inc
    ${PA_ROOT}/domain/inc
    ${PA_ROOT}/ports/application/comm
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/application/comm
)

add_test(
    NAME comm_application
    COMMAND test_comm_application
)


# =========================================================
# DHT11 ADAPTER
# =========================================================

add_executable(test_dht11_adapter
    unit/adapters/dht11/test_dht11_adapter.c
    unit/adapters/dht11/dht11_port_mock.c
    ${PA_ROOT}/adapters/dht11/src/dht11_adapter.c
)

target_include_directories(test_dht11_adapter PRIVATE
    ${PA_ROOT}/adapters/dht11/inc
    ${PA_ROOT}/ports/application/sensor
    ${PA_ROOT}/ports/hardware/dht11
    ${PA_ROOT}/domain/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/adapters/dht11
)

add_test(
    NAME dht11_adapter
    COMMAND test_dht11_adapter
)


# =========================================================
# SERIAL ADAPTER
# =========================================================

add_executable(test_serial_adapter
    unit/adapters/serial/test_serial_adapter.c
    unit/adapters/serial/serial_port_mock.c
    ${PA_ROOT}/adapters/serial/src/serial_adapter.c
)

target_include_directories(test_serial_adapter PRIVATE
    ${PA_ROOT}/adapters/serial/inc
    ${PA_ROOT}/ports/application/comm
    ${PA_ROOT}/ports/hardware/serial
    ${PA_ROOT}/domain/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/adapters/serial
)

add_test(
    NAME serial_adapter
    COMMAND test_serial_adapter
)


# =========================================================
# DHT11 HARDWARE PORT
# =========================================================

add_executable(test_dht11_port
    unit/ports/hardware/dht11/test_dht11_port.c
)

target_include_directories(test_dht11_port PRIVATE
    ${PA_ROOT}/ports/hardware/dht11
)

add_test(
    NAME dht11_port
    COMMAND test_dht11_port
)


# =========================================================
# SERIAL HARDWARE PORT
# =========================================================

add_executable(test_serial_port
    unit/ports/hardware/serial/test_serial_port.c
)

target_include_directories(test_serial_port PRIVATE
    ${PA_ROOT}/ports/hardware/serial
)

add_test(
    NAME serial_port
    COMMAND test_serial_port
)


# =========================================================
# SENSOR APPLICATION PORT
# =========================================================

add_executable(test_sensor_port
    unit/ports/application/sensor/test_sensor_port.c
)

target_include_directories(test_sensor_port PRIVATE
    ${PA_ROOT}/ports/application/sensor
    ${PA_ROOT}/domain/inc
)

add_test(
    NAME sensor_port
    COMMAND test_sensor_port
)


# =========================================================
# COMM APPLICATION PORT
# =========================================================

add_executable(test_comm_port
    unit/ports/application/comm/test_comm_port.c
)

target_include_directories(test_comm_port PRIVATE
    ${PA_ROOT}/ports/application/comm
    ${PA_ROOT}/domain/inc
)

add_test(
    NAME comm_port
    COMMAND test_comm_port
)

# =========================================================
# SENSOR -> SERIAL INTEGRATION TEST
# =========================================================

add_executable(test_sensor_to_serial

    integration/sensor_to_serial/test_sensor_to_serial.c
    integration/sensor_to_serial/dht11_integration_mock.c

    ${PA_ROOT}/domain/src/temperature_domain.c

    ${PA_ROOT}/application/src/sensor_application.c
    ${PA_ROOT}/application/src/comm_application.c

    ${PA_ROOT}/adapters/dht11/src/dht11_adapter.c
    ${PA_ROOT}/adapters/serial/src/serial_adapter.c
)


target_include_directories(test_sensor_to_serial PRIVATE

    ${PA_ROOT}/domain/inc

    ${PA_ROOT}/ports/application/sensor
    ${PA_ROOT}/ports/application/comm

    ${PA_ROOT}/ports/hardware/dht11
    ${PA_ROOT}/ports/hardware/serial

    ${PA_ROOT}/application/inc

    ${PA_ROOT}/adapters/dht11/inc
    ${PA_ROOT}/adapters/serial/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/integration/sensor_to_serial
)


add_test(
    NAME sensor_to_serial
    COMMAND test_sensor_to_serial
)

# =========================================================
# SOFTWARE-IN-THE-LOOP (SIL)
# Sensor System
# =========================================================

add_executable(test_sensor_system_sil

    sil/sensor_system/test_sensor_system_sil.c

    sil/sensor_system/dht11_sil_model.c
    sil/sensor_system/serial_sil_model.c

    # -----------------------------------------------------
    # Domain
    # -----------------------------------------------------

    ../ports_and_adapters/domain/src/temperature_domain.c

    # -----------------------------------------------------
    # Applications
    # -----------------------------------------------------

    ../ports_and_adapters/application/src/sensor_application.c
    ../ports_and_adapters/application/src/comm_application.c

    # -----------------------------------------------------
    # Adapters
    # -----------------------------------------------------

    ../ports_and_adapters/adapters/dht11/src/dht11_adapter.c
    ../ports_and_adapters/adapters/serial/src/serial_adapter.c
)


target_include_directories(test_sensor_system_sil PRIVATE

    # -----------------------------------------------------
    # Domain
    # -----------------------------------------------------

    ../ports_and_adapters/domain/inc

    # -----------------------------------------------------
    # Application
    # -----------------------------------------------------

    ../ports_and_adapters/application/inc

    # -----------------------------------------------------
    # Application Ports
    # -----------------------------------------------------

    ../ports_and_adapters/ports/application/sensor
    ../ports_and_adapters/ports/application/comm

    # -----------------------------------------------------
    # Hardware Ports
    # -----------------------------------------------------

    ../ports_and_adapters/ports/hardware/dht11
    ../ports_and_adapters/ports/hardware/serial

    # -----------------------------------------------------
    # Adapters
    # -----------------------------------------------------

    ../ports_and_adapters/adapters/dht11/inc
    ../ports_and_adapters/adapters/serial/inc

    # -----------------------------------------------------
    # SIL Models
    # -----------------------------------------------------

    sil/sensor_system
)


# =========================================================
# CTest
# =========================================================

add_test(
    NAME sensor_system_sil
    COMMAND test_sensor_system_sil
)
