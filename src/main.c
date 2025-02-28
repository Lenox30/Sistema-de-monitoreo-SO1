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

#include "expose_metrics.h" // Para exponer las métricas vía HTTP
#include "globant.h"        // Para inicializar las métricas
#include <cjson/cJSON.h>    // Para manejar JSON
#include <stdbool.h>        // Para usar valores booleanos

/**
 * @brief Actualiza las métricas del sistema según la configuración proporcionada.
 *
 * @param config Estructura de configuración que contiene los parámetros necesarios para actualizar las métricas.
 */
void update_metrics(Config config);

/**
 * @brief Carga la configuración desde un archivo.
 *
 * @param filename Ruta del archivo de configuración.
 * @return Config Estructura de configuración cargada desde el archivo.
 */
Config load_config(const char* filename);

/**
 * @brief Limpia la configuración al final del programa.
 *
 * @param config Estructura de configuración a limpiar.
 */
void clean_config(Config* config);

/**
 * @brief Función principal
 * @param argc Cantidad de argumentos
 * @param argv Argumentos de la línea de comandos
 * @return 0 si la ejecución fue exitosa, 1 en caso contrario
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    // Cargar la configuración
    Config config = load_config("../config.json");

    // Inicializamos Metricas
    init_metrics(config);
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;

    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Creamos un hilo para actualizar las métricas
    pthread_t update_tid;

    while (true)
    {
        update_metrics(config);
        sleep((unsigned int)config.sampling_interval);
    }

    // Limpiar configuración al final del programa
    clean_config(&config);

    // Esperamos a que los hilos terminen (aunque en este caso, no lo harán)
    pthread_join(tid, NULL);
    pthread_join(update_tid, NULL);

    return EXIT_SUCCESS;
}

/**
 * @brief Actualiza las métricas del sistema según la configuración proporcionada.
 *
 * @param config Estructura de configuración que contiene los parámetros necesarios para actualizar las métricas.
 */
void update_metrics(Config config)
{
    // Actualizar las métricas según la configuración
    for (int i = 0; i < config.metrics_count; i++)
    {
        if (strcmp(config.metrics[i], "cpu_usage") == 0)
        {
            update_cpu_gauge();
        }
        else if (strcmp(config.metrics[i], "memory_usage") == 0)
        {
            update_memory_gauge();
        }
        else if (strcmp(config.metrics[i], "disk_usage") == 0)
        {
            update_disk_gauge();
        }
        else if (strcmp(config.metrics[i], "network_usage") == 0)
        {
            update_network_gauge();
        }
        else if (strcmp(config.metrics[i], "running_processes") == 0)
        {
            update_proccess_gauge();
        }
        else if (strcmp(config.metrics[i], "context_switches") == 0)
        {
            update_context_switches_gauge();
        }
        else if (strcmp(config.metrics[i], "First_Fit") == 0)
        {
            update_First_Fit_gauge();
        }
        else if (strcmp(config.metrics[i], "Best_Fit") == 0)
        {
            update_Best_Fit_gauge();
        }
        else if (strcmp(config.metrics[i], "Worst_Fit") == 0)
        {
            update_Worst_Fit_gauge();
        }
        // Agregar más métricas según sea necesario
    }
}

/**
 * @brief Carga la configuración desde un archivo.
 *
 * @param filename Ruta del archivo de configuración.
 * @return Config Estructura de configuración cargada desde el archivo.
 */
Config load_config(const char* filename)
{
    Config config = {1, NULL, 0};

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error al abrir el archivo de configuración");
        return config;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json_data = (char*)malloc((size_t)length + 1);
    if (json_data == NULL)
    {
        perror("Error al asignar memoria para el archivo JSON");
        fclose(file);
        return config;
    }

    if (fread(json_data, 1, (size_t)length, file) != (size_t)length)
    {
        perror("Error al leer el archivo de configuración");
        free(json_data);
        json_data = NULL;
        fclose(file);
        return config;
    }
    fclose(file);
    json_data[length] = '\0';

    cJSON* json = cJSON_Parse(json_data);
    free(json_data);
    json_data = NULL;
    if (json == NULL)
    {
        fprintf(stderr, "Error al parsear el JSON: %s\n", cJSON_GetErrorPtr());
        return config;
    }

    cJSON* sampling_interval = cJSON_GetObjectItem(json, "sampling_interval");
    if (cJSON_IsNumber(sampling_interval))
    {
        config.sampling_interval = sampling_interval->valueint;
    }

    cJSON* metrics = cJSON_GetObjectItem(json, "metrics");
    if (cJSON_IsArray(metrics))
    {
        config.metrics_count = cJSON_GetArraySize(metrics);
        config.metrics =
            (char**)calloc((size_t)config.metrics_count, sizeof(char*)); // Usar calloc para inicializar a NULL
        if (config.metrics == NULL)
        {
            perror("Error al asignar memoria para las métricas");
            cJSON_Delete(json);
            return config;
        }

        for (int i = 0; i < config.metrics_count; i++)
        {
            cJSON* metric = cJSON_GetArrayItem(metrics, i);
            if (cJSON_IsString(metric))
            {
                config.metrics[i] = strdup(metric->valuestring); // Copiar el nombre de la métrica
                if (config.metrics[i] == NULL)
                {
                    perror("Error al asignar memoria para una métrica");

                    // Limpiar lo ya asignado
                    for (int j = 0; j < i; j++)
                    {
                        free(config.metrics[j]);
                        config.metrics[j] = NULL;
                    }
                    free(config.metrics);
                    config.metrics = NULL;

                    config.metrics = NULL;
                    config.metrics_count = 0;

                    cJSON_Delete(json);
                    return config; // Retornar configuración por defecto
                }
            }
        }
    }

    cJSON_Delete(json);
    return config;
}

/**
 * @brief Limpia la configuración al final del programa.
 *
 * @param config Estructura de configuración a limpiar.
 */
void clean_config(Config* config)
{
    if (config->metrics != NULL)
    {
        for (int i = 0; i < config->metrics_count; i++)
        {
            if (config->metrics[i] != NULL)
            { // Validar antes de liberar
                free(config->metrics[i]);
                config->metrics[i] = NULL; // Evitar dobles liberaciones
            }
        }
        free(config->metrics);
        config->metrics = NULL;
    }
    config->metrics_count = 0;
}