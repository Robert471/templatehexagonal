# Sensor de Temperatura y Humedad — STM32F401RE

Proyecto embebido en **C** para la adquisición de temperatura y humedad mediante un sensor **DHT11**, ejecutado sobre un **STM32F401RE**, utilizando **Arquitectura Hexagonal (Ports & Adapters)**, principios **SOLID** y una organización orientada a **MISRA-C**.

El proyecto incluye pruebas **unitarias, integración, SIL (Software-in-the-Loop) y HIL (Hardware-in-the-Loop)**.

---

## 📌 Descripción

El sistema realiza las siguientes funciones:

1. Inicializa los periféricos del STM32.
2. Lee temperatura y humedad desde el DHT11.
3. Clasifica la temperatura según las reglas del dominio.
4. Formatea el resultado.
5. Envía la información mediante **USART2**.
6. Permite validar el comportamiento mediante pruebas SIL.
7. Permite validar el comportamiento sobre hardware real mediante HIL.

El resultado puede observarse mediante el puerto serie **COM8**.

Ejemplo:

```text
Temperature: 28 C
```

---

## 🎯 Hardware

| Componente | Configuración |
|---|---|
| MCU | STM32F401RE |
| Board | STM32 Nucleo |
| Sensor | DHT11 |
| DHT11 Data | PC3 |
| Timer | TIM3 |
| UART | USART2 |
| Puerto PC | COM8 |
| Baudrate | 115200 |
| Data bits | 8 |
| Paridad | None |
| Stop bits | 1 |

La configuración concreta del hardware se realiza en el **Composition Root**, manteniendo desacopladas las capas de dominio y aplicación.

---

# 🏗️ Arquitectura

El proyecto utiliza **Arquitectura Hexagonal (Ports & Adapters)**.

```text
                    ┌─────────────────────┐
                    │       DOMAIN        │
                    │                     │
                    │ TemperatureData     │
                    │ TemperatureLevel    │
                    │ Business Rules      │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │    APPLICATION      │
                    │                     │
                    │ Sensor Application  │
                    │ Comm Application    │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │       PORTS         │
                    │                     │
                    │ Sensor Port         │
                    │ Comm Port           │
                    │ DHT11 Port          │
                    │ Serial Port         │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │      ADAPTERS       │
                    │                     │
                    │ DHT11 Adapter       │
                    │ Serial Adapter      │
                    │ LCD Adapter         │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │      DRIVERS        │
                    │                     │
                    │ GPIO Driver         │
                    │ Timer Driver        │
                    │ USART Driver        │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │    STM32 HAL        │
                    └─────────────────────┘
```

Regla principal de dependencia:

```text
Domain
   ↓
Application
   ↓
Ports
   ↓
Adapters
   ↓
Drivers
   ↓
STM32 HAL
```

El **Composition Root** es el encargado de ensamblar las implementaciones concretas.

---

# 📂 Estructura del proyecto

```text
templatehexagonal/
│
├── Core/
├── Drivers/
│
├── ports_and_adapters/
│   ├── domain/
│   ├── application/
│   ├── ports/
│   ├── adapters/
│   ├── drivers/
│   └── composition/
│
├── tests/
│   ├── unit/
│   ├── integration/
│   ├── sil/
│   └── hil/
│
├── cmake/
├── doc/
│
├── CMakeLists.txt
├── CMakePresets.json
├── STM32F401RECmake.ioc
├── STM32F401xx_FLASH.ld
├── startup_stm32f401xe.s
└── README.md
```

---

# 🔗 Acceso rápido a los archivos

## Firmware STM32

- [Core](./Core)
- [STM32 HAL / Drivers](./Drivers)
- [CMake principal](./CMakeLists.txt)
- [CMake Presets](./CMakePresets.json)
- [STM32CubeMX `.ioc`](./STM32F401RECmake.ioc)
- [Linker Script](./STM32F401xx_FLASH.ld)
- [Startup STM32F401](./startup_stm32f401xe.s)

---

## Domain

La capa de dominio contiene las reglas de negocio y los modelos independientes del hardware.

- [Domain](./ports_and_adapters/domain)
- [Temperature Domain Header](./ports_and_adapters/domain/inc/temperature_domain.h)
- [Temperature Domain Implementation](./ports_and_adapters/domain/src/temperature_domain.c)

---

# Application

La capa Application coordina los casos de uso sin conocer directamente STM32 HAL.

- [Application](./ports_and_adapters/application)
- [Sensor Application Header](./ports_and_adapters/application/inc/sensor_application.h)
- [Sensor Application](./ports_and_adapters/application/src/sensor_application.c)
- [Communication Application Header](./ports_and_adapters/application/inc/comm_application.h)
- [Communication Application](./ports_and_adapters/application/src/comm_application.c)

---

# Ports

Los Ports definen los contratos utilizados entre la aplicación y la infraestructura.

## Application Ports

- [Sensor Port](./ports_and_adapters/ports/application/sensor/sensor_port.h)
- [Communication Port](./ports_and_adapters/ports/application/comm/comm_port.h)

## Hardware Ports

- [DHT11 Port](./ports_and_adapters/ports/hardware/dht11/dht11_port.h)
- [Serial Port](./ports_and_adapters/ports/hardware/serial/serial_port.h)

Los Ports permiten sustituir las implementaciones concretas por mocks y modelos SIL durante las pruebas.

---

# Adapters

Los adapters implementan la lógica de adaptación entre los casos de uso y los Ports de infraestructura.

## DHT11 Adapter

- [DHT11 Adapter Header](./ports_and_adapters/adapters/dht11/inc/dht11_adapter.h)
- [DHT11 Adapter](./ports_and_adapters/adapters/dht11/src/dht11_adapter.c)

Implementa el protocolo de comunicación con el DHT11 mediante el contrato definido por `Dht11_Port`.

## Serial Adapter

- [Serial Adapter Header](./ports_and_adapters/adapters/serial/inc/serial_adapter.h)
- [Serial Adapter](./ports_and_adapters/adapters/serial/src/serial_adapter.c)

Se encarga del formateo de los datos antes de enviarlos por el Port serial.

## LCD Adapter

- [LCD Adapter Header](./ports_and_adapters/adapters/lcd/inc/lcd_adapter.h)
- [LCD Adapter](./ports_and_adapters/adapters/lcd/src/lcd_adapter.c)

---

# Drivers

Los drivers contienen la interacción concreta con STM32 HAL.

- [GPIO Driver](./ports_and_adapters/drivers/stm32/gpio)
- [Timer Driver](./ports_and_adapters/drivers/stm32/timer)
- [USART Driver](./ports_and_adapters/drivers/stm32/usart)

Los drivers encapsulan las llamadas específicas a STM32 HAL para evitar que las capas superiores dependan directamente de ellas.

---

# Composition Root

La composición de dependencias se realiza en:

- [Sensor Composition](./ports_and_adapters/composition/sensor)
- [Communication Composition](./ports_and_adapters/composition/comm)

El Composition Root es el lugar donde se conocen las implementaciones concretas de hardware.

---

# 🧪 Testing

El proyecto dispone de diferentes niveles de prueba.

```text
Unit Tests
     ↓
Integration Tests
     ↓
SIL
     ↓
HIL
     ↓
Hardware real
```

---

## Unit Tests

Ubicación:

[tests/unit](./tests/unit)

Se prueban individualmente:

- Domain
- Application
- Ports
- Adapters

Ejemplos:

- [Temperature Domain Test](./tests/unit/domain/temperature/test_temperature_domain.c)
- [Sensor Application Test](./tests/unit/application/sensor/test_sensor_application.c)
- [Communication Application Test](./tests/unit/application/comm/test_comm_application.c)
- [DHT11 Adapter Test](./tests/unit/adapters/dht11/test_dht11_adapter.c)
- [Serial Adapter Test](./tests/unit/adapters/serial/test_serial_adapter.c)

---

## Integration Test

El flujo Sensor → Serial se valida mediante:

[Sensor to Serial Integration Test](./tests/integration/sensor_to_serial)

Este test verifica la integración entre:

```text
DHT11
 ↓
Sensor Application
 ↓
Communication Application
 ↓
Serial Adapter
```

---

# 🖥️ SIL — Software-in-the-Loop

Los tests SIL se encuentran en:

[tests/sil](./tests/sil)

El sistema utiliza modelos de software para simular:

- DHT11
- Serial

Archivos principales:

- [DHT11 SIL Model](./tests/sil/sensor_system/dht11_sil_model.c)
- [Serial SIL Model](./tests/sil/sensor_system/serial_sil_model.c)
- [Sensor System SIL Test](./tests/sil/sensor_system/test_sensor_system_sil.c)

---

# 🔌 HIL — Hardware-in-the-Loop

La prueba HIL utiliza hardware real.

Script:

[run_hil.py](./tests/hil/run_hil.py)

Ejecutar desde la raíz del proyecto:

```powershell
py tests\hil\run_hil.py
```

El script realiza el flujo de validación del firmware:

```text
CMake
  ↓
Build firmware
  ↓
HEX
  ↓
Programación STM32
  ↓
USART2 / COM8
  ↓
DHT11
  ↓
Validación de muestras
```

Resultado esperado:

```text
========================================
          HIL TEST PASSED
========================================
```

Una ejecución validada sobre hardware real produjo:

```text
Promedio: 28.00 C

HIL TEST PASSED
```

---

# 🔨 Compilación

## Firmware

El proyecto utiliza CMake y el toolchain ARM GCC:

[ARM GCC Toolchain](./cmake/gcc-arm-none-eabi.cmake)

Configuración típica:

```powershell
cmake -S . -B build\Debug -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=cmake\gcc-arm-none-eabi.cmake `
  -DCMAKE_BUILD_TYPE=Debug
```

Compilación:

```powershell
cmake --build build\Debug
```

---

# 🧪 Compilar y ejecutar tests

Los tests tienen su propio [CMakeLists.txt](./tests/CMakeLists.txt).

Desde la raíz:

```powershell
cmake -S tests -B build-tests
```

Compilar:

```powershell
cmake --build build-tests --config Debug
```

Ejecutar:

```powershell
ctest --test-dir build-tests -C Debug --output-on-failure
```

Resultado esperado:

```text
100% tests passed
```

---

# 📐 Principios de diseño

## Hexagonal Architecture

La arquitectura separa:

- Domain
- Application
- Ports
- Adapters
- Drivers
- Composition Root

El hardware concreto queda aislado en la infraestructura.

## SOLID

### Single Responsibility Principle

Cada componente tiene una responsabilidad específica:

```text
Domain      → reglas de negocio
Application → casos de uso
Port        → contrato
Adapter     → adaptación
Driver      → hardware
Composition → ensamblaje
```

### Dependency Inversion Principle

Application depende de Ports y no de implementaciones concretas.

Las implementaciones se inyectan desde el Composition Root.

### Open/Closed Principle

Los Ports permiten reemplazar implementaciones sin modificar las capas superiores.

Esto se utiliza directamente en los tests SIL e integración.

---

# 🛡️ MISRA-C

El código de producción está organizado siguiendo prácticas orientadas a **MISRA-C**, incluyendo:

- tipos enteros explícitos (`uint8_t`, `uint16_t`, `uint32_t`)
- uso controlado de conversiones
- uso de `const`
- validación de parámetros
- inicialización explícita
- reducción de dependencias implícitas
- separación de responsabilidades
- encapsulamiento de hardware
- ausencia de dependencia de `<stdio.h>` en el Serial Adapter

Las APIs externas de STM32 HAL constituyen la frontera de hardware del sistema.

> La conformidad formal con MISRA-C requiere ejecutar una herramienta de análisis estático certificada y documentar las desviaciones aplicables.

---

# 📊 Estado de validación

| Validación | Estado |
|---|---|
| Unit Tests | ✅ PASS |
| Integration Tests | ✅ PASS |
| SIL | ✅ PASS |
| HIL | ✅ PASS |
| STM32F401RE | ✅ Validado |
| DHT11 | ✅ Validado |
| USART2 | ✅ Validado |
| COM8 | ✅ Validado |
| Arquitectura Hexagonal | ✅ |
| SOLID | ✅ |
| MISRA-oriented | ✅ |

Última validación HIL:

```text
Promedio: 28.00 C
HIL TEST PASSED
```

---

# 📚 Documentación de hardware

- [STM32F401RE Nucleo Pinout](./doc/STM32F401RENUCLEO_pinout.png)
- [STM32 Nucleo-64 User Manual](./doc/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)
- [STM32CubeMX Configuration](./STM32F401RECmake.ioc)

---

# 🚀 CI/CD

El proyecto queda preparado para incorporar automatización CI/CD mediante GitHub Actions.

Estrategia prevista para CI:

```text
Git Push / Pull Request
        ↓
      Build
        ↓
   Unit / Integration
        ↓
       SIL
        ↓
 Static Analysis
        ↓
       PASS
```

Estrategia prevista para HIL:

```text
Scheduled Workflow
        ↓
Self-hosted Runner
        ↓
Build Firmware
        ↓
Program STM32
        ↓
Execute run_hil.py
        ↓
DHT11 + USART2
        ↓
HIL PASS / FAIL
```

---

# 👤 Autor

**Robert Ramirez**

Proyecto desarrollado para STM32F401RE utilizando C, STM32 HAL, CMake, Arquitectura Hexagonal, SOLID y pruebas SIL/HIL.
