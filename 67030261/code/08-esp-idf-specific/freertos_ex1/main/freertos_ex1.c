#include <stdio.h>

void app_main(void)
{

}
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "DUAL_CORE_EX1";
#define COMPUTE_ITERATIONS 100000
#define QUEUE_LENGTH 10

QueueHandle_t intercore_queue;

// ==================================================
// Core 0 - Compute-Intensive Task
// ==================================================
void compute_task(void *param)
{
    uint32_t iteration = 0;
    int core = xPortGetCoreID();

    while (1)
    {
        // simulate compute-heavy load
        volatile float result = 0;
        for (int i = 0; i < COMPUTE_ITERATIONS; i++)
        {
            result += i * 0.0001f;
        }

        iteration++;
        ESP_LOGI(TAG, "Core %d compute task: iteration %u", core, iteration);

        // Send message to Core 1
        if (xQueueSend(intercore_queue, &iteration, 0) == pdTRUE)
        {
            ESP_LOGI(TAG, "Inter-core message: Core 0 -> Core 1");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // run every second
    }
}

// ==================================================
// Core 1 - I/O & Communication Task
// ==================================================
void io_task(void *param)
{
    int core = xPortGetCoreID();
    uint32_t received_value;

    while (1)
    {
        if (xQueueReceive(intercore_queue, &received_value, pdMS_TO_TICKS(2000)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Core %d I/O task: processing data (%u)", core, received_value);
        }
        else
        {
            ESP_LOGW(TAG, "Core %d I/O task: waiting for data...", core);
        }
    }
}

// ==================================================
// CPU Utilization Monitor (Simulated)
// ==================================================
void cpu_monitor_task(void *param)
{
    while (1)
    {
        // ในระบบจริงสามารถใช้ vTaskGetRunTimeStats() ได้
        // ที่นี่จำลองค่าการใช้ CPU เพื่อสาธิต
        float core0_usage = 75.0;
        float core1_usage = 45.0;

        ESP_LOGI(TAG, "CPU utilization - Core 0: %.0f%%, Core 1: %.0f%%", core0_usage, core1_usage);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ==================================================
// app_main()
// ==================================================
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Exercise 1: Dual-Core Task Distribution");

    intercore_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));

    // Core 0: Compute Task
    xTaskCreatePinnedToCore(
        compute_task,
        "ComputeTask",
        4096,
        NULL,
        5,
        NULL,
        0 // PRO_CPU (Core 0)
    );

    // Core 1: I/O Task
    xTaskCreatePinnedToCore(
        io_task,
        "IOTask",
        4096,
        NULL,
        4,
        NULL,
        1 // APP_CPU (Core 1)
    );

    // Monitor CPU utilization
    xTaskCreate(
        cpu_monitor_task,
        "CPUMonitor",
        4096,
        NULL,
        3,
        NULL
    );
}
