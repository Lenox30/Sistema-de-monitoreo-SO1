/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#ifndef METRICS_H // Directiva para evitar inclusiones múltiples
#define METRICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer
 */
#define BUFFER_SIZE 256 // Tamaño del buffer

/**
 * @brief Política de asignación de memoria
 */
#define FIRST_FIT 0

/**
 * @brief Política de asignación de memoria
 */
#define BEST_FIT 1

/**
 * @brief Política de asignación de memoria
 */
#define WORST_FIT 2

/**
 * @brief Ruta del FIFO
 */
#define FIFO_PATH "/tmp/my_fifo" // Define the FIFO path

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
 * @brief Estructura para almacenar los resultados de la evaluación de una política de asignación de memoria.
 */
typedef struct
{
    char policy_name[20];         /**< Nombre de la política de asignación de memoria */
    int iterations;               /**< Número de iteraciones */
    float time_taken;             /**< Tiempo de ejecución */
    size_t total_allocated;       /**< Memoria total asignada */
    int freed_blocks;             /**< Bloques liberados */
    int free_blocks;              /**< Bloques libres */
    size_t free_size;             /**< Tamaño total de memoria libre */
    float avg_fragmentation;      /**< Fragmentación promedio */
    float external_fragmentation; /**< Fragmentación externa */
} MemoryMetrics;

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
 * @brief Obtiene el porcentaje de uso de memoria.
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
 * Pide los valores de memoria libre y total, luego calcula la memoria usada en kB.
 *
 * @return Memoria usada en KB, o -1.0 en caso de error.
 */
double get_memory_used(void);

/**
 * @brief Obtiene la tasa de fragmentación de memoria.
 *
 * Pide los valores de memoria total, libre y usada, luego calcula la tasa de fragmentación.
 *
 * @return Tasa de fragmentación de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_fragmentation(void);

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

/**
 * @brief Obtiene las métricas de la política de asignación de memoria First Fit.
 *
 * Evalúa la política de asignación de memoria First Fit y retorna los resultados.
 */
int get_First_Fit(MemoryMetrics*);

/**
 * @brief Obtiene las métricas de la política de asignación de memoria Best Fit.
 *
 * Evalúa la política de asignación de memoria Best Fit y retorna los resultados.
 */
int get_Best_Fit(MemoryMetrics*);

/**
 * @brief Obtiene las métricas de la política de asignación de memoria Worst Fit.
 *
 * Evalúa la política de asignación de memoria Worst Fit y retorna los resultados.
 */
int get_Worst_Fit(MemoryMetrics*);

/**
 * @brief Obtiene las métricas de la política de asignación de memoria.
 *
 * Evalúa la política de asignación de memoria y retorna los resultados.
 *
 * @param policy Política de asignación de memoria (FIRST_FIT, BEST_FIT o WORST_FIT).
 * @param metrics Estructura para almacenar los resultados de la evaluación.
 * @return 0 en caso de éxito, -1 en caso de error.
 */
void read_memory_metrics(void);

/**
 * @brief Ejecuta la política de asignación de memoria.
 *
 * Ejecuta la política de asignación de memoria seleccionada.
 *
 * @param policy Política de asignación de memoria (FIRST_FIT, BEST_FIT o WORST_FIT).
 */
void ejecutar_memory(int);

#endif // METRICS_H
