#ifndef GLOBANT_H
#define GLOBANT_H
/*
 * @brief estructura de configuracion
 *
 */
typedef struct
{
    int sampling_interval; // Intervalo de muestreo
    char** metrics;        // Nombres de las métricas
    int metrics_count;     // Cantidad de métricas
} Config;

/**
 * @brief Estructura de configuración
 *
 * Esta estructura contiene la configuración del sistema, incluyendo el intervalo de muestreo
 * y las métricas a monitorear.
 */
#define intervalo 10

/**
 * @brief Tamaño del buffer
 * Tamaño del buffer para leer archivos en el sistema de archivos.
 */
#define BUFFER_SIZE 256

#endif // GLOBANT_H