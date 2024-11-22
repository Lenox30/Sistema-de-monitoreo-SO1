/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer
 * Tamaño del buffer
 */
#define BUFFER_SIZE 256 // Tamaño del buffer

/**
 * @brief Estructura para almacenar las métricas del disco.
 */
typedef struct
{
    unsigned long read_time_ms;   /**< Tiempo de lectura del disco en ms */
    unsigned long write_time_ms;  /**< Tiempo de escritura del disco en ms */
    unsigned long io_in_progress; /**< Número de operaciones de E/S en progreso */
    unsigned long io_time_ms;     /**< Tiempo de E/S del disco en ms */
} DiskMetrics;

/**
 * @brief Estructura para almacenar las métricas de red.
 */
typedef struct
{
    char interface[32];             /**< Nombre de la interfaz de red */
    unsigned long receive_bytes;    /**< Bytes recibidos por la interfaz */
    unsigned long transmit_bytes;   /**< Bytes transmitidos por la interfaz */
    unsigned long receive_errors;   /**< Errores de recepción */
    unsigned long transmit_errors;  /**< Errores de transmisión */
    unsigned long receive_dropped;  /**< Paquetes recibidos descartados */
    unsigned long transmit_dropped; /**< Paquetes transmitidos descartados */
} NetworkMetrics;

/**
 * @brief Obtiene la cantidad de cambios de contexto.
 *
 * Lee el archivo /proc/stat y obtiene la cantidad de cambios de contexto.
 *
 * @return Cantidad de cambios de contexto, o -1 en caso de error.
 */
long long get_context_switches(void);

/**
 * @brief Obtiene la cantidad de procesos en ejecución.
 *
 * Lee el archivo /proc/loadavg y obtiene la cantidad de procesos en ejecución.
 *
 * @return Cantidad de procesos en ejecución, o -1 en caso de error.
 */
int get_running_processes(void);

/**
 * @brief Obtiene las métricas de red desde /proc/net/dev.
 *
 * Lee las métricas de red desde /proc/net/dev y las almacena en una estructura.
 *
 * @param metrics Estructura para almacenar las métricas de red.
 */
int get_network_metrics(NetworkMetrics*);

/**
 * @brief Obtiene el procentaje de uso de memoria.
 *
 * Pide los valores a get_memory_free y get_memory_total, luego calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage(void);

/**
 * @brief Obtiene el valor de uso de memoria.
 *
 * Lee los valores de memoria total/proc/meminfo y calcula
 * el valor en kB del uso de memoria.
 *
 * @return Memoria total en KB, o -1.0 en caso de error.
 */
double get_memory_total(void);

/**
 * @brief Obtiene el valor de la memoria libre.
 *
 * Lee los valores de memoria disponible en /proc/meminfo y calcula
 * el valor en kB de la memoria disponible.
 *
 * @return Memoria libre en KB, o -1.0 en caso de error.
 */
double get_memory_free(void);

/**
 * @brief Obtiene el valor de la memoria usada.
 *
 * Pide los valores de memoria libre y total, luego calcula la memoria usada en kB
 *
 * @return Memoria usada en KB, o -1.0 en caso de error.
 */
double get_memory_used(void);

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage(void);
/**
 * @brief Obtiene las métricas de disco desde /proc/diskstats.
 *
 * Lee las métricas de disco desde /proc/diskstats y las almacena en una estructura.
 *
 * @param metrics Estructura para almacenar las métricas de disco.
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int get_disk_metrics(DiskMetrics*);
