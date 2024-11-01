/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU y memoria y exponerlos como métricas de Prometheus.
 */

#include "metrics.h"
// #include "read_cpu_usage.h"
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para sleep

/**
 * @brief Tamaño del buffer
 * Tamaño del buffer
 */
#define BUFFER_SIZE 256 // Tamaño del buffer

/**
 * @brief Actualiza la métrica de uso de CPU.
 */
void update_cpu_gauge();

/**
 * @brief Actualiza la métrica de uso de memoria.
 */
void update_memory_gauge();

/**
 * @brief Actualiza las métricas de disco.
 */
void update_disk_gauge();

/**
 * @brief Actualiza las métricas de red.
 */
void update_network_gauge();

/**
 * @brief Actualiza la métrica de procesos en ejecución.
 */
void update_proccess_gauge();

/**
 * @brief Actualiza la métrica de cambios de contexto.
 */
void update_context_switches_gauge();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 * @param arg Argumento no utilizado.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializar mutex y métricas.
 */
void init_metrics();

/**
 * @brief Destructor de mutex
 */
void destroy_mutex();
