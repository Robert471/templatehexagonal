cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32F401RECmake)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    

    ports_and_adapters/application/src/sensor_application.c
    ports_and_adapters/application/src/comm_application.c

    
    ports_and_adapters/domain/src/temperature_domain.c

    ports_and_adapters/adapters/serial/src/serial_adapter.c
    ports_and_adapters/adapters/lcd/src/lcd_adapter.c   
    ports_and_adapters/adapters/dht11/src/dht11_adapter.c   
    


    ports_and_adapters/drivers/stm32/gpio/src/gpio_driver.c
    ports_and_adapters/drivers/stm32/timer/src/timer_driver.c
    ports_and_adapters/drivers/stm32/usart/src/usart_driver.c

    ports_and_adapters/composition/comm/src/comm_composition.c
    ports_and_adapters/composition/sensor/src/sensor_composition.c



   

    #ports_and_adapters/ports/dht11/src/dht11_port.c
    #ports_and_adapters/ports/serial/src/serial_port.c
    #ports_and_adapters/ports/lcd/src/lcd_port.c
    
    #lib/src/output.c 
    #lib/src/rs232.c
    #lib/src/dht11.c
    #lib/src/lcd.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    #${CMAKE_SOURCE_DIR}/lib/inc
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/adapters/serial/inc
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/adapters/lcd/inc 
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/adapters/dht11/inc

    ${CMAKE_SOURCE_DIR}/ports_and_adapters/application/inc

    ${CMAKE_SOURCE_DIR}/ports_and_adapters/domain/inc


    ${CMAKE_SOURCE_DIR}/ports_and_adapters/drivers/stm32/gpio/inc
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/drivers/stm32/usart/inc
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/drivers/stm32/timer/inc

    ${CMAKE_SOURCE_DIR}/ports_and_adapters/ports/hardware/dht11
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/ports/hardware/serial
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/ports/application/sensor

    ${CMAKE_SOURCE_DIR}/ports_and_adapters/composition/comm/inc
    ${CMAKE_SOURCE_DIR}/ports_and_adapters/composition/sensor/inc    

)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# ============================================================
# GENERAR ARCHIVO HEX DESPUÉS DE COMPILAR
# ============================================================

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD

    COMMAND ${CMAKE_OBJCOPY}
            -O ihex
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex

    COMMENT "Generating ${CMAKE_PROJECT_NAME}.hex"
)