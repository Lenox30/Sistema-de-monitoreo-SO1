cmake_minimum_required(VERSION 3.10)
project(MetricsProject)

# Incluye las bibliotecas que necesitas (ejemplo: cJSON)
find_package(cJSON REQUIRED)

# Agrega los archivos de tu proyecto al ejecutable
add_executable(MetricsProject src/main.c src/metrics.c src/expose_metrics.c)

# Incluye los directorios de encabezados
target_include_directories(MetricsProject PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Enlaza las bibliotecas necesarias al objetivo `MetricsProject`
target_link_libraries(MetricsProject PRIVATE cJSON)

# Otras opciones de configuración, si es necesario
set_target_properties(MetricsProject PROPERTIES C_STANDARD 11)
