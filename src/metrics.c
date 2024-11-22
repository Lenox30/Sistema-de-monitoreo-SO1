/**
 * @file metrics.c
 * @brief Implementación de funciones de métricas del sistema.
 *
 * Este archivo contiene funciones para recuperar varias métricas del sistema como uso de memoria,
 * uso de CPU, métricas de disco, métricas de red, procesos en ejecución y cambios de contexto.
 */

#include "metrics.h"

// Función para obtener la memoria total en MB
double get_memory_total(void)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem_aux = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0; // Retornar -1 en caso de error
    }

    // Leer los valores de memoria total
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem_aux) == 1)
        {
            break; // MemTotal encontrado, salir del bucle
        }
    }

    fclose(fp);

    // Verificar si se encontró el valor
    if (total_mem_aux == 0)
    {
        fprintf(stderr, "Error al leer la memoria total desde /proc/meminfo\n");
        return -1.0; // Retornar -1 en caso de error
    }

    // Convertir de kB a MB
    return (double)total_mem_aux / 1024.0; // Retornar la memoria total en MB
}

// Función para obtener la memoria libre en MB
double get_memory_free(void)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long free_mem_aux = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0; // Retornar -1 en caso de error
    }

    // Leer los valores de memoria libre
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem_aux) == 1)
        {
            break; // MemAvailable encontrado, salir del bucle
        }
    }

    fclose(fp);

    // Verificar si se encontró el valor
    if (free_mem_aux == 0)
    {
        fprintf(stderr, "Error al leer la memoria libre desde /proc/meminfo\n");
        return -1.0; // Retornar -1 en caso de error
    }

    // Convertir de kB a MB
    return (double)free_mem_aux / 1024.0; // Retornar la memoria libre en MB
}

// Función para calcular el uso de memoria
double get_memory_usage(void)
{
    double total_mem = get_memory_total();
    double free_mem = get_memory_free();

    // Calcular el porcentaje de uso de memoria
    if (total_mem > 0)
    {
        double used_mem = total_mem - free_mem;
        return (used_mem / total_mem) * 100.0; // Retornar el porcentaje de uso de memoria
    }

    return -1.0; // Error si total_mem es 0
}

// Función para obtener la memoria usada en MB
double get_memory_used(void)
{
    double mem_used = 0;
    double free_mem = get_memory_free();
    double total_mem = get_memory_total();
    mem_used = total_mem - free_mem;
    return mem_used;
}

// Función para obtener el uso de CPU
double get_cpu_usage(void)
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / (double)totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

// Función para leer las métricas de disco desde /proc/diskstats
int get_disk_metrics(DiskMetrics* metrics)
{
    FILE* file = fopen("/proc/diskstats", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    const char* device_name = "sda";
    while (fgets(buffer, BUFFER_SIZE, file))
    {
        if (strstr(buffer, device_name) != NULL)
        {
            // Utilizamos strtok para dividir la línea en campos
            char* token = strtok(buffer, " ");
            int field = 0;
            while (token != NULL)
            {
                // Avanzamos a través de los campos
                if (field == 9)
                { // Campo 8: io_in_progress
                    metrics->io_in_progress = strtoul(token, NULL, 10);
                }
                else if (field == 4)
                { // Campo 9: read_time_ms
                    metrics->read_time_ms = strtoul(token, NULL, 10);
                }
                else if (field == 8)
                { // Campo 10: write_time_ms
                    metrics->write_time_ms = strtoul(token, NULL, 10);
                }
                else if (field == 10)
                { // Campo 11: io_time_ms
                    metrics->io_time_ms = strtoul(token, NULL, 10);
                }
                token = strtok(NULL, " ");
                field++;
            }

            fclose(file);
            return 0; // Éxito
        }
    }

    fclose(file);
    return -1; // Dispositivo no encontrado
}

// Función para extraer las métricas desde /proc/net/dev
int get_network_metrics(NetworkMetrics* metrics)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    char* token;
    int buffer_num = 0;

    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1;
    }

    // Salta las dos primeras bufferas de encabezado
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        if (buffer_num < 2)
        {
            buffer_num++;
            continue;
        }

        // Tokenizar la línea por espacios
        token = strtok(buffer, ":");
        if (token)
        {
            // Obtener nombre de la interfaz
            strcpy(metrics->interface, token);

            // Continuar con los valores de la interfaz (primero es el de Receive)
            token = strtok(NULL, " ");
            if (token)
            {
                // Leer los datos de Receive (primero están los bytes, luego errs y drop)
                metrics->receive_bytes = strtoul(token, NULL, 10);
                for (int i = 0; i < 2; i++)
                    token = strtok(NULL, " ");                                   // Saltar packets
                metrics->receive_errors = strtoul(token, NULL, 10);              // Tercer valor (errs)
                metrics->receive_dropped = strtoul(strtok(NULL, " "), NULL, 10); // Cuarto valor (drop)

                // Saltar los campos restantes de Receive
                for (int i = 0; i < 5; i++)
                    token = strtok(NULL, " ");

                // Leer los datos de Transmit (nuevamente bytes, errs y drop)
                metrics->transmit_bytes = strtoul(token, NULL, 10); // Bytes de Transmit
                for (int i = 0; i < 2; i++)
                    token = strtok(NULL, " ");                                    // Saltar packets
                metrics->transmit_errors = strtoul(token, NULL, 10);              // Errores de Transmit
                metrics->transmit_dropped = strtoul(strtok(NULL, " "), NULL, 10); // Paquetes descartados de Transmit
            }
        }

        buffer_num++;
    }

    fclose(fp);
    return 0;
}

// Función para obtener el número de procesos en ejecución
int get_running_processes(void)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    int running_processes = 0;

    // Abrir el archivo /proc/loadavg
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1; // Retornar -1 en caso de error
    }

    // Leer los valores de /proc/stat
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Buscar la línea que comienza con "procs_running"
        if (strncmp(buffer, "procs_running", 13) == 0)
        {
            // Extraer el número de procesos en ejecución
            sscanf(buffer, "procs_running %d", &running_processes);
            break;
        }
    }

    // Verificar si se encontró el valor
    if (running_processes == 0)
    {
        fprintf(stderr, "Error al leer el número de procesos en ejecución desde /proc/stat\n");
        fclose(fp);
        return -1; // Retornar -1 en caso de error
    }

    fclose(fp);

    return running_processes;
}

// Función para obtener la cantidad de cambios de contexto
long long get_context_switches(void)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long context_switches = 0;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1;
    }

    // Leer las líneas de /proc/stat
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Buscar la línea que comienza con "ctxt"
        if (strncmp(buffer, "ctxt", 4) == 0)
        {
            // Parsear la línea y obtener el número de cambios de contexto
            sscanf(buffer, "ctxt %llu", &context_switches);
            break;
        }
    }

    fclose(fp);
    return (long long)context_switches;
}
