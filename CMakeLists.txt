cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(MetricsApp)

# Añadir el directorio de inclusión
include_directories(include)

# Establecer la ruta donde están las bibliotecas
link_directories(lib/Prometheus-client-c)  # Ajusta la ruta según donde estén tus bibliotecas

# Incluir los directorios de encabezados de Prometheus
include_directories(lib/Prometheus-client-c/prom)  # Ajusta para que incluya los headers

# Enlazar las bibliotecas necesarias
target_link_libraries(metrics PRIVATE prom pthread promhttp)

# Definir los archivos fuente
set(SRC_FILES
    src/expose_metrics.c
    src/metrics.c
    src/main.c
)

# Crear el ejecutable
add_executable(metrics ${SRC_FILES})

# Enlazar bibliotecas necesarias
target_link_libraries(metrics prom pthread promhttp)

# Establecer el directorio de salida
set_target_properties(metrics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

