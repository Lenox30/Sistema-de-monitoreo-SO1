/**
 * @file expose_metrics.c
 * @brief Implementación de las funciones para exponer métricas de sistema vía HTTP
 */

#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria en %*/
static prom_gauge_t* memory_usage_metric;
/** Métrica de Prometheus para el total de memoria */
static prom_gauge_t* total_memory_metric;
/** Métrica de Prometheus para memoria libre*/
static prom_gauge_t* available_memory_metric;
/** Métrica de Prometheus para memoria usada en KB*/
static prom_gauge_t* used_memory_metric;

/** Métrica de Prometheus para el tiempo de lectura del disco */
static prom_gauge_t* disk_read_time_metric;
/** Métrica de Prometheus para el tiempo de escritura del disco */
static prom_gauge_t* disk_write_time_metric;
/** Métrica de Prometheus para el número de operaciones de E/S en progreso */
static prom_gauge_t* disk_io_in_progress_metric;
/** Métrica de Prometheus para el tiempo de E/S del disco */
static prom_gauge_t* disk_io_time_metric;
/** Estructura para almacenar las métricas de disco */
DiskMetrics metrics_disk;

/** Métrica de Prometheus para los bytes recibidos por la red */
static prom_gauge_t* network_received_bytes_metric;
/** Métrica de Prometheus para los bytes transmitidos por la red */
static prom_gauge_t* network_transmitted_bytes_metric;
/**Metrica de Prometheus para errores recibidos*/
static prom_gauge_t* network_received_errors_metric;
/**Metrica de Prometheus para errores transmitidos*/
static prom_gauge_t* network_transmitted_errors_metric;
/**Metrica de Prometheus para paquetes recibidos*/
static prom_gauge_t* network_received_dropped_metric;
/**Metrica de Prometheus para paquetes transmitidos*/
static prom_gauge_t* network_transmitted_dropped_metric;
/** Estructura para almacenar las métricas de red */
NetworkMetrics metrics_network;

/** Métrica de Prometheus para el número de procesos en ejecución */
static prom_gauge_t* running_processes_metric;

/** Métrica de Prometheus para la cantidad de cambios de contexto */
static prom_gauge_t* context_switches_metric;

// Actualiza la métrica de uso de CPU
int update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);

        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
        return EXIT_FAILURE;
    }
}

// Actualiza las métricas de memoria
int update_memory_gauge()
{
    double usage = get_memory_usage();
    double total = get_memory_total();
    double used = get_memory_used();
    double available = get_memory_free();
    if (usage >= 0 && total >= 0 && used >= 0 && available >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        prom_gauge_set(total_memory_metric, total, NULL);
        prom_gauge_set(used_memory_metric, used, NULL);
        prom_gauge_set(available_memory_metric, available, NULL);
        pthread_mutex_unlock(&lock);
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
        return EXIT_FAILURE;
    }
}

// Actualiza las métricas de disco
void update_disk_gauge()
{

    int disk_metrics = get_disk_metrics(&metrics_disk);
    if (disk_metrics >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(disk_read_time_metric, (double)metrics_disk.read_time_ms, NULL);
        prom_gauge_set(disk_write_time_metric, (double)metrics_disk.write_time_ms, NULL);
        prom_gauge_set(disk_io_in_progress_metric, (double)metrics_disk.io_in_progress, NULL);
        prom_gauge_set(disk_io_time_metric, (double)metrics_disk.io_time_ms, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener las métricas de disco\n");
    }
}

// Actualiza las métricas de red
void update_network_gauge()
{
    int network_metrics = get_network_metrics(&metrics_network);
    if (network_metrics >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(network_received_bytes_metric, (double)metrics_network.receive_bytes, NULL);
        prom_gauge_set(network_transmitted_bytes_metric, (double)metrics_network.transmit_bytes, NULL);
        prom_gauge_set(network_received_errors_metric, (double)metrics_network.receive_errors, NULL);
        prom_gauge_set(network_transmitted_errors_metric, (double)metrics_network.transmit_errors, NULL);
        prom_gauge_set(network_received_dropped_metric, (double)metrics_network.receive_dropped, NULL);
        prom_gauge_set(network_transmitted_dropped_metric, (double)metrics_network.transmit_dropped, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener las métricas de red\n");
    }
}

// Actualiza la métrica de procesos en ejecución
void update_proccess_gauge()
{
    int running_processes = get_running_processes();
    if (running_processes >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(running_processes_metric, running_processes, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el número de procesos en ejecución\n");
    }
}

// Actualiza la métrica de cambios de contexto
void update_context_switches_gauge()
{
    double context_switches = (double)get_context_switches();
    if (context_switches >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, context_switches, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de cambios de contexto\n");
    }
}

// Función del hilo para exponer las métricas vía HTTP en el puerto 8000
void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

// Inicializar mutex y métricas
int init_metrics(Config config)
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return EXIT_FAILURE;
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el uso de CPU
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
        return EXIT_FAILURE;
    }

    // Creamos las métricas de memoria (total, usada, disponible)
    total_memory_metric = prom_gauge_new("total_memory_mb", "Memoria total en MB", 0, NULL);
    used_memory_metric = prom_gauge_new("used_memory_mb", "Memoria usada en MB", 0, NULL);
    available_memory_metric = prom_gauge_new("available_memory_mb", "Memoria disponible en MB", 0, NULL);
    if (total_memory_metric == NULL || used_memory_metric == NULL || available_memory_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métricas de memoria\n");
        return EXIT_FAILURE;
    }

    // Creamos las métricas de disco
    disk_read_time_metric = prom_gauge_new("disk_read_time_ms", "Tiempo de lectura del disco en ms", 0, NULL);
    disk_write_time_metric = prom_gauge_new("disk_write_time_ms", "Tiempo de escritura del disco en ms", 0, NULL);
    disk_io_in_progress_metric = prom_gauge_new("disk_io_in_progress", "Operaciones de E/S en progreso", 0, NULL);
    disk_io_time_metric = prom_gauge_new("disk_io_time_ms", "Tiempo de E/S del disco en ms", 0, NULL);
    if (disk_read_time_metric == NULL || disk_write_time_metric == NULL || disk_io_in_progress_metric == NULL ||
        disk_io_time_metric == NULL)
    {
        fprintf(stderr, "Error al crear las métricas de disco\n");
        return EXIT_FAILURE;
    }

    // Creamos las métricas de red
    network_received_bytes_metric = prom_gauge_new("network_received_bytes", "Bytes recibidos por la red", 0, NULL);
    network_transmitted_bytes_metric =
        prom_gauge_new("network_transmitted_bytes", "Bytes transmitidos por la red", 0, NULL);
    network_received_errors_metric = prom_gauge_new("network_received_errors", "Errores recibidos por la red", 0, NULL);
    network_transmitted_errors_metric =
        prom_gauge_new("network_transmitted_errors", "Errores transmitidos por la red", 0, NULL);
    network_received_dropped_metric =
        prom_gauge_new("network_received_dropped", "Paquetes recibidos por la red", 0, NULL);
    network_transmitted_dropped_metric =
        prom_gauge_new("network_transmitted_dropped", "Paquetes transmitidos por la red", 0, NULL);
    if (network_received_bytes_metric == NULL || network_transmitted_bytes_metric == NULL ||
        network_received_errors_metric == NULL || network_transmitted_errors_metric == NULL ||
        network_received_dropped_metric == NULL || network_transmitted_dropped_metric == NULL)
    {
        fprintf(stderr, "Error al crear las métricas de red\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el número de procesos en ejecución
    running_processes_metric = prom_gauge_new("running_processes", "Número de procesos en ejecución", 0, NULL);
    if (running_processes_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de procesos en ejecución\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para la cantidad de cambios de contexto
    context_switches_metric = prom_gauge_new("context_switches", "Cantidad de cambios de contexto", 0, NULL);
    if (context_switches_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cambios de contexto\n");
        return EXIT_FAILURE;
    }

    // Registramos las métricas en el registro por defecto
    // Actualizar las métricas según la configuración
    for (int i = 0; i < config.metrics_count; i++)
    {
        if (strcmp(config.metrics[i], "cpu_usage") == 0)
        {
            if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar la métrica de uso de CPU\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(config.metrics[i], "memory_usage") == 0)
        {
            if (prom_collector_registry_must_register_metric(memory_usage_metric) == NULL ||
                prom_collector_registry_must_register_metric(total_memory_metric) == NULL ||
                prom_collector_registry_must_register_metric(used_memory_metric) == NULL ||
                prom_collector_registry_must_register_metric(available_memory_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar las métricas de memoria\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(config.metrics[i], "disk_usage") == 0)
        {
            if (prom_collector_registry_must_register_metric(disk_read_time_metric) == NULL ||
                prom_collector_registry_must_register_metric(disk_write_time_metric) == NULL ||
                prom_collector_registry_must_register_metric(disk_io_in_progress_metric) == NULL ||
                prom_collector_registry_must_register_metric(disk_io_time_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar las métricas de disco\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(config.metrics[i], "network_usage") == 0)
        {
            if (prom_collector_registry_must_register_metric(network_received_bytes_metric) == NULL ||
                prom_collector_registry_must_register_metric(network_transmitted_bytes_metric) == NULL ||
                prom_collector_registry_must_register_metric(network_received_errors_metric) == NULL ||
                prom_collector_registry_must_register_metric(network_transmitted_errors_metric) == NULL ||
                prom_collector_registry_must_register_metric(network_received_dropped_metric) == NULL ||
                prom_collector_registry_must_register_metric(network_transmitted_dropped_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar las métricas de red\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(config.metrics[i], "running_processes") == 0)
        {
            if (prom_collector_registry_must_register_metric(running_processes_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar la métrica de procesos en ejecución\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(config.metrics[i], "context_switches") == 0)
        {
            if (prom_collector_registry_must_register_metric(context_switches_metric) == NULL)
            {
                fprintf(stderr, "Error al registrar la métrica de cambios de contexto\n");
                return EXIT_FAILURE;
            }
        }
        // Agregar más métricas según sea necesario
    }
    return EXIT_SUCCESS;
}

// Destructor de mutex
void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
