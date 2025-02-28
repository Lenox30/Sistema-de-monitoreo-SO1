cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(MetricsProject)

# Configuración de compilación
set(CMAKE_BUILD_TYPE Debug)

# Habilitar depuración con símbolos completos y sin optimización
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_STANDARD 17)

# Incluir directorios de encabezados
include_directories(include)
include_directories(include) # Encabezados del proyecto principal

# Añadir los archivos fuente al proyecto
set(SOURCES
    src/metrics.c
    src/main.c
    src/expose_metrics.c
)

# Crear el ejecutable
add_executable(metrics ${SOURCES})

# Especificar bibliotecas externas
find_package(cJSON REQUIRED)
find_library(PROM_LIB prom REQUIRED)
find_library(PROMHTTP_LIB promhttp REQUIRED)

# Enlazar las bibliotecas necesarias
target_link_libraries(metrics PRIVATE ${PROM_LIB} ${PROMHTTP_LIB} pthread cjson::cjson memory)

# Configurar el directorio de salida del ejecutable
set_target_properties(metrics PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Habilitar depuración en GDB/LLDB
set_target_properties(metrics PROPERTIES
    COMPILE_FLAGS "-ggdb"
)

# Mensaje de recordatorio para LD_LIBRARY_PATH
message(STATUS "Para ejecutar, asegúrate de tener configurado LD_LIBRARY_PATH:")
message(STATUS "export LD_LIBRARY_PATH=/usr/local/lib:\$LD_LIBRARY_PATH")
