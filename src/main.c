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
#include <cjson/cJSON.h> // Para manejar JSON

/**
 * @brief Tamaño del buffer
 * Tamaño del buffer para leer archivos en el sistema de archivos.
 */
#define BUFFER_SIZE 256
/**
 * @brief Intervalo de muestreo por defecto
 * Intervalo de muestreo por defecto en segundos.
 */
#define DEFAULT_SAMPLING_INTERVAL 1

/*
 * @brief estructura de configuracion
 * 
 */
typedef struct {
    int sampling_interval; // Intervalo de muestreo
    char** metrics; // Nombres de las métricas
    int metrics_count; // Cantidad de métricas
} Config;

//funcion para actualizar las metricas
void update_metrics(Config config);

/**
 * @brief Función principal
 * @param argc Cantidad de argumentos
 * @param argv Argumentos de la línea de comandos
 * @return 0 si la ejecución fue exitosa, 1 en caso contrario
 */
int main(int argc, char* argv[])
{
    // Cargar la configuración
    Config config = load_config("config.json");

    // Inicializamos Metricas
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;

    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Creamos un hilo para actualizar las métricas
    pthread_t update_tid;

    while (true) {
        update_metrics(config);
        sleep(config.sampling_interval);
    }

    // Limpiar
    for (int i = 0; i < config.metrics_count; i++) {
        free(config.metrics[i]);
    }
    free(config.metrics);

    // Esperamos a que los hilos terminen (aunque en este caso, no lo harán)
    pthread_join(tid, NULL);
    pthread_join(update_tid, NULL);

    return EXIT_SUCCESS;
}

    void update_metrics(Config config) {
    for (int i = 0; i < config.metrics_count; i++) { 
        if (strcmp(config.metrics[i], "cpu_usage") == 0) {
            update_cpu_gauge();
        } else if (strcmp(config.metrics[i], "memory_usage") == 0) {
            update_memory_gauge();
        } else if (strcmp(config.metrics[i], "disk_usage") == 0) {
            update_disk_gauge();
        } else if (strcmp(config.metrics[i], "network_usage") == 0) {
            update_network_gauge();
        } else if (strcmp(config.metrics[i], "running_processes") == 0) {
            update_proccess_gauge();
        } else if (strcmp(config.metrics[i], "context_switches") == 0) {
            update_context_switches_gauge();
        }
        // Agregar más métricas según sea necesario
    }
}

// Función para cargar la configuración desde un archivo JSON
Config load_config(const char* filename) {
    Config config = {DEFAULT_SAMPLING_INTERVAL, NULL, 0};
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        return config; // Retornar la configuración por defecto
    }

    // Leer el contenido del archivo
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* json_data = (char*)malloc(length + 1);
    fread(json_data, 1, length, file);
    fclose(file);
    json_data[length] = '\0';

    // Parsear el JSON
    cJSON* json = cJSON_Parse(json_data);
    if (json == NULL) {
        fprintf(stderr, "Error al parsear el JSON: %s\n", cJSON_GetErrorPtr());
        free(json_data);
        return config; // Retornar la configuración por defecto
    }

    // Obtener el intervalo de muestreo
    cJSON* sampling_interval = cJSON_GetObjectItem(json, "sampling_interval");
    if (cJSON_IsNumber(sampling_interval)) {
        config.sampling_interval = sampling_interval->valueint;
    }

    // Obtener las métricas
    cJSON* metrics = cJSON_GetObjectItem(json, "metrics");
    if (cJSON_IsArray(metrics)) {
        config.metrics_count = cJSON_GetArraySize(metrics);
        config.metrics = malloc(config.metrics_count * sizeof(char*));
        for (int i = 0; i < config.metrics_count; i++) {
            cJSON* metric = cJSON_GetArrayItem(metrics, i);
            config.metrics[i] = strdup(metric->valuestring); // Copiar el nombre de la métrica
        }
    }

    // Limpiar
    cJSON_Delete(json);
    free(json_data);
    return config;
}