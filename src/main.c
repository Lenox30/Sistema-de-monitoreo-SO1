/**
 * @file main.c
 * @brief Entry point of the system
 *
 * Este archivo contiene la función principal que inicializa y actualiza las métricas del sistema.
 * Crea un hilo separado para exponer estas métricas vía HTTP.
 *
 * El bucle principal actualiza varias métricas del sistema cada segundo, incluyendo:
 * - Uso de CPU
 * - Uso de memoria
 * - Uso de disco
 * - Uso de red
 * - Conteo de procesos
 * - Cambios de contexto
 *
 * Las métricas se exponen usando la función `expose_metrics` en un hilo separado.
 *
 * @note El programa se ejecutará indefinidamente, actualizando las métricas cada segundo.
 *
 * @see expose_metrics.h
 */

#include "expose_metrics.h"
#include <stdbool.h>

/**
 * @brief Tiempo de espera entre actualizaciones de métricas
 * Tiempo de espera entre actualizaciones de métricas en segundos.
 */
#define SLEEP_TIME 1

/**
 * @brief Función principal
 * @param argc Cantidad de argumentos
 * @param argv Argumentos de la línea de comandos
 * @return 0 si la ejecución fue exitosa, 1 en caso contrario
 */
int main(int argc, char* argv[])
{
    // Inicializamos Metricas
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;

    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Bucle principal para actualizar las métricas cada segundo
    while (true)
    {
        update_cpu_gauge();
        update_memory_gauge();
        update_disk_gauge();
        update_network_gauge();
        update_proccess_gauge();
        update_context_switches_gauge();
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}