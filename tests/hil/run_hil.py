import os
import sys
import time
import shutil
import subprocess
import re

import serial
from serial import SerialException


# ============================================================
# CONFIGURACIÓN
# ============================================================

# tests/hil/run_hil.py
#        ↑
#        └── ../.. = raíz del proyecto

PROJECT_ROOT = os.path.abspath(
    os.path.join(
        os.path.dirname(__file__),
        "..",
        ".."
    )
)

BUILD_DIR = os.path.join(
    PROJECT_ROOT,
    "build",
    "Debug"
)

TOOLCHAIN_FILE = os.path.join(
    PROJECT_ROOT,
    "cmake",
    "gcc-arm-none-eabi.cmake"
)

SERIAL_PORT = "COM8"
BAUDRATE = 115200

SAMPLES_REQUIRED = 5
TIMEOUT_SECONDS = 15

MIN_TEMPERATURE = 0
MAX_TEMPERATURE = 50


# ============================================================
# UTILIDADES
# ============================================================

def print_title(text):
    print()
    print("=" * 60)
    print(text)
    print("=" * 60)


def command_exists(command):
    return shutil.which(command) is not None


def run_command(command, description):

    print()
    print("-" * 60)
    print(description)
    print("-" * 60)

    print()
    print("Ejecutando:")
    print(" ".join(command))
    print()

    try:

        process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding="utf-8",
            errors="replace"
        )

        for line in process.stdout:
            print(line.rstrip())

        process.wait()

        if process.returncode == 0:

            print()
            print(">>> PASS")

            return True

        print()
        print(
            f">>> FAIL "
            f"(codigo {process.returncode})"
        )

        return False

    except FileNotFoundError as error:

        print()
        print(">>> FAIL")
        print()
        print("No se encontró el ejecutable:")
        print(error)

        return False

    except Exception as error:

        print()
        print(">>> FAIL")
        print()
        print("Error:")
        print(error)

        return False


# ============================================================
# 1. COMPILAR FIRMWARE
# ============================================================

def build_firmware():

    print_title(
        "[1/5] COMPILANDO FIRMWARE"
    )

    # --------------------------------------------------------
    # Verificar CMake
    # --------------------------------------------------------

    if not command_exists("cmake"):

        print(
            "ERROR: cmake no está disponible "
            "en PATH."
        )

        return False

    # --------------------------------------------------------
    # Verificar Ninja
    # --------------------------------------------------------

    if not command_exists("ninja"):

        print(
            "ERROR: ninja no está disponible "
            "en PATH."
        )

        return False

    # --------------------------------------------------------
    # Verificar toolchain
    # --------------------------------------------------------

    if not os.path.isfile(TOOLCHAIN_FILE):

        print()
        print(
            "ERROR: no existe el toolchain:"
        )
        print(
            TOOLCHAIN_FILE
        )

        return False

    # --------------------------------------------------------
    # Mostrar información
    # --------------------------------------------------------

    print()
    print(
        f"Proyecto : {PROJECT_ROOT}"
    )

    print(
        f"Build    : {BUILD_DIR}"
    )

    print(
        f"Toolchain: {TOOLCHAIN_FILE}"
    )

    # --------------------------------------------------------
    # Eliminar build/Debug anterior
    #
    # Si no existe, no pasa nada.
    # CMake lo creará.
    # --------------------------------------------------------

    if os.path.exists(BUILD_DIR):

        print()
        print(
            "Eliminando build/Debug anterior..."
        )

        try:

            shutil.rmtree(BUILD_DIR)

        except Exception as error:

            print()
            print(
                "ERROR eliminando build/Debug:"
            )

            print(error)

            return False

    # --------------------------------------------------------
    # Crear configuración CMake
    #
    # IMPORTANTE:
    #
    # build/Debug NO necesita existir.
    #
    # CMake lo crea.
    # --------------------------------------------------------

    configure_command = [

        "cmake",

        f"-DCMAKE_TOOLCHAIN_FILE={TOOLCHAIN_FILE}",

        "-DCMAKE_BUILD_TYPE=Debug",

        "-S",
        PROJECT_ROOT,

        "-B",
        BUILD_DIR,

        "-G",
        "Ninja"
    ]

    if not run_command(
        configure_command,
        "Configurando CMake..."
    ):

        return False

    # --------------------------------------------------------
    # Compilar
    # --------------------------------------------------------

    build_command = [

        "cmake",

        "--build",

        BUILD_DIR
    ]

    if not run_command(
        build_command,
        "Compilando firmware..."
    ):

        return False

    return True


# ============================================================
# 2. BUSCAR HEX
# ============================================================

def find_hex():

    print_title(
        "[2/5] BUSCANDO FIRMWARE HEX"
    )

    if not os.path.isdir(BUILD_DIR):

        print()
        print(
            "ERROR: no existe:"
        )

        print(
            BUILD_DIR
        )

        return None

    hex_files = []

    for filename in os.listdir(BUILD_DIR):

        if filename.lower().endswith(".hex"):

            path = os.path.join(
                BUILD_DIR,
                filename
            )

            if os.path.isfile(path):

                hex_files.append(path)

    if not hex_files:

        print()
        print(
            "ERROR: no se encontró ningún "
            ".hex en build/Debug."
        )

        return None

    # Si hubiera varios HEX,
    # usar el más reciente.

    hex_files.sort(
        key=os.path.getmtime,
        reverse=True
    )

    hex_file = hex_files[0]

    print()
    print(
        "HEX encontrado:"
    )

    print(
        hex_file
    )

    return hex_file


# ============================================================
# 3. PROGRAMAR STM32
# ============================================================

def flash_stm32(hex_file):

    print_title(
        "[3/5] PROGRAMANDO STM32F401RE"
    )

    programmer = shutil.which(
        "STM32_Programmer_CLI"
    )

    if programmer is None:

        programmer = shutil.which(
            "STM32_Programmer_CLI.exe"
        )

    if programmer is None:

        print()
        print(
            "ERROR: STM32_Programmer_CLI "
            "no está disponible en PATH."
        )

        print()
        print(
            "Prueba en PowerShell:"
        )

        print()
        print(
            "STM32_Programmer_CLI"
        )

        return False

    print()
    print(
        "Programador:"
    )

    print(
        programmer
    )

    print()
    print(
        "Firmware:"
    )

    print(
        hex_file
    )

    # --------------------------------------------------------
    # EXACTAMENTE el mismo método que usas
    # en tu programa actual.
    # --------------------------------------------------------

    command = [

        programmer,

        "-c",
        "port=SWD",

        "-d",
        hex_file,

        "-rst"
    ]

    return run_command(
        command,
        "Flasheando mediante ST-LINK..."
    )


# ============================================================
# 4. ABRIR COM8
# ============================================================

def open_serial():

    print_title(
        "[4/5] ABRIENDO COM8"
    )

    print()
    print(
        f"Puerto   : {SERIAL_PORT}"
    )

    print(
        f"Baudrate : {BAUDRATE}"
    )

    try:

        ser = serial.Serial(

            port=SERIAL_PORT,

            baudrate=BAUDRATE,

            bytesize=serial.EIGHTBITS,

            parity=serial.PARITY_NONE,

            stopbits=serial.STOPBITS_ONE,

            timeout=1
        )

        print()
        print(
            ">>> COM8 ABIERTO"
        )

        return ser

    except SerialException as error:

        print()
        print(
            ">>> FAIL"
        )

        print()
        print(
            "No se pudo abrir COM8:"
        )

        print(error)

        return None


# ============================================================
# 5. TEST HIL
# ============================================================

def execute_hil(ser):

    print_title(
        "[5/5] EJECUTANDO HIL"
    )

    print()
    print(
        "Esperando datos reales del STM32..."
    )

    print()
    print(
        "Formato esperado:"
    )

    print(
        "Temperature: XX C"
    )

    print()

    # --------------------------------------------------------
    # Limpiar datos anteriores
    # --------------------------------------------------------

    ser.reset_input_buffer()

    # --------------------------------------------------------
    # Regex
    # --------------------------------------------------------

    temperature_regex = re.compile(
        r"Temperature:\s*(-?\d+)\s*C",
        re.IGNORECASE
    )

    temperatures = []

    start_time = time.time()

    # --------------------------------------------------------
    # Recepción
    # --------------------------------------------------------

    while True:

        elapsed = (
            time.time()
            -
            start_time
        )

        if elapsed >= TIMEOUT_SECONDS:

            print()
            print(
                "TIMEOUT"
            )

            break

        try:

            raw_line = ser.readline()

        except SerialException as error:

            print()
            print(
                "ERROR leyendo COM8:"
            )

            print(error)

            return False

        if not raw_line:

            continue

        line = raw_line.decode(
            "utf-8",
            errors="replace"
        ).strip()

        if not line:

            continue

        print(
            f"RX -> {line}"
        )

        # ----------------------------------------------------
        # Buscar temperatura
        # ----------------------------------------------------

        match = temperature_regex.search(
            line
        )

        if match is None:

            print(
                "     mensaje no reconocido"
            )

            continue

        try:

            temperature = int(
                match.group(1)
            )

        except ValueError:

            print(
                "     temperatura inválida"
            )

            continue

        # ----------------------------------------------------
        # Validar rango
        # ----------------------------------------------------

        if (
            temperature < MIN_TEMPERATURE
            or
            temperature > MAX_TEMPERATURE
        ):

            print(
                f"     FAIL "
                f"({temperature} C fuera de rango)"
            )

            continue

        # ----------------------------------------------------
        # Muestra válida
        # ----------------------------------------------------

        temperatures.append(
            temperature
        )

        print(
            f"     PASS "
            f"({temperature} C)"
        )

        # ----------------------------------------------------
        # ¿Tenemos suficientes?
        # ----------------------------------------------------

        if len(temperatures) >= SAMPLES_REQUIRED:

            break

    # --------------------------------------------------------
    # Resultado
    # --------------------------------------------------------

    print()
    print(
        "-" * 60
    )

    print(
        "RESULTADOS"
    )

    print(
        "-" * 60
    )

    print(
        f"Muestras requeridas : "
        f"{SAMPLES_REQUIRED}"
    )

    print(
        f"Muestras válidas    : "
        f"{len(temperatures)}"
    )

    if temperatures:

        print()
        print(
            "Temperaturas recibidas:"
        )

        for index, temperature in enumerate(
            temperatures,
            start=1
        ):

            print(
                f"  {index}. "
                f"{temperature} C"
            )

        average = (
            sum(temperatures)
            /
            len(temperatures)
        )

        print()
        print(
            f"Promedio: "
            f"{average:.2f} C"
        )

    print()

    # --------------------------------------------------------
    # PASS
    # --------------------------------------------------------

    if len(temperatures) >= SAMPLES_REQUIRED:

        print(
            "========================================"
        )

        print(
            "          HIL TEST PASSED"
        )

        print(
            "========================================"
        )

        return True

    # --------------------------------------------------------
    # FAIL
    # --------------------------------------------------------

    print(
        "========================================"
    )

    print(
        "          HIL TEST FAILED"
    )

    print(
        "========================================"
    )

    return False


# ============================================================
# MAIN
# ============================================================

def main():

    print_title(
        "SENSOR HUMEDAD - HIL AUTOMATED TEST"
    )

    print()
    print(
        f"Proyecto: {PROJECT_ROOT}"
    )

    print(
        f"STM32: NUCLEO-F401RE"
    )

    print(
        f"ST-LINK: SWD"
    )

    print(
        f"Serial: {SERIAL_PORT}"
    )

    # --------------------------------------------------------
    # BUILD
    # --------------------------------------------------------

    if not build_firmware():

        print()
        print(
            "HIL ABORTADO"
        )

        return 1

    # --------------------------------------------------------
    # HEX
    # --------------------------------------------------------

    hex_file = find_hex()

    if hex_file is None:

        print()
        print(
            "HIL ABORTADO"
        )

        return 1

    # --------------------------------------------------------
    # FLASH
    # --------------------------------------------------------

    if not flash_stm32(
        hex_file
    ):

        print()
        print(
            "HIL ABORTADO"
        )

        return 1

    # --------------------------------------------------------
    # Esperar reset
    # --------------------------------------------------------

    print()
    print(
        "Esperando reinicio del STM32..."
    )

    time.sleep(2)

    # --------------------------------------------------------
    # SERIAL
    # --------------------------------------------------------

    ser = open_serial()

    if ser is None:

        print()
        print(
            "HIL ABORTADO"
        )

        return 1

    try:

        result = execute_hil(
            ser
        )

    finally:

        ser.close()

        print()
        print(
            "COM8 cerrado."
        )

    # --------------------------------------------------------
    # RESULTADO FINAL
    # --------------------------------------------------------

    print()

    if result:

        print(
            "########################################"
        )

        print(
            "#                                      #"
        )

        print(
            "#            HIL TEST PASSED           #"
        )

        print(
            "#                                      #"
        )

        print(
            "########################################"
        )

        return 0

    print(
        "########################################"
    )

    print(
        "#                                      #"
    )

    print(
        "#            HIL TEST FAILED           #"
    )

    print(
        "#                                      #"
    )

    print(
        "########################################"
    )

    return 1


# ============================================================
# ENTRY POINT
# ============================================================

if __name__ == "__main__":

    sys.exit(
        main()
    )
