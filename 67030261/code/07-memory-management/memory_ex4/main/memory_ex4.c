#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

static const char *TAG = "MEM_MONITOR_EX4";

// ===============================
// 1️⃣ Heap & Stack Monitoring
// ===============================
void memory_monitor_task(void *pv)
{
    multi_heap_info_t heap_info;
    while (1)
    {
        heap_caps_get_info(&heap_info, MALLOC_CAP_DEFAULT);

        size_t free_heap = xPortGetFreeHeapSize();
        size_t min_free_heap = xPortGetMinimumEverFreeHeapSize();

        ESP_LOGI(TAG, "=== MEMORY STATUS ===");
        ESP_LOGI(TAG, "Free heap: %d bytes", free_heap);
        ESP_LOGI(TAG, "Min ever free heap: %d bytes", min_free_heap);
        ESP_LOGI(TAG, "Largest free block: %d bytes", heap_info.largest_free_block);
        ESP_LOGI(TAG, "Allocated blocks: %d", heap_info.allocated_blocks);
        ESP_LOGI(TAG, "Free blocks: %d", heap_info.free_blocks);
        ESP_LOGI(TAG, "====================");

        // เตือนเมื่อ heap ต่ำ
        if (free_heap < 10000)
            ESP_LOGW(TAG, "⚠️ LOW MEMORY WARNING: %d bytes free", free_heap);

        // ตรวจสอบ stack ของทุก task
        UBaseType_t task_count = uxTaskGetNumberOfTasks();
        TaskStatus_t *status_array = pvPortMalloc(task_count * sizeof(TaskStatus_t));
        if (status_array)
        {
            UBaseType_t tasks = uxTaskGetSystemState(status_array, task_count, NULL);
            for (UBaseType_t i = 0; i < tasks; i++)
            {
                UBaseType_t remaining = uxTaskGetStackHighWaterMark(status_array[i].xHandle);
                if (remaining < 300)
                {
                    ESP_LOGW(TAG, "Task [%s] LOW STACK: %d bytes remaining", status_array[i].pcTaskName, remaining);
                }
            }
            vPortFree(status_array);
        }

        vTaskDelay(pdMS_TO_TICKS(10000)); // ตรวจทุก 10 วินาที
    }
}

// ===============================
// 2️⃣ Memory Stress Simulation
// ===============================
void memory_stress_task(void *pv)
{
    while (1)
    {
        void *buf = pvPortMalloc(2048);
        if (buf)
        {
            memset(buf, 0xAA, 2048);
            ESP_LOGI(TAG, "Allocated 2KB buffer at %p", buf);
            vTaskDelay(pdMS_TO_TICKS(1000));
            vPortFree(buf);
            ESP_LOGI(TAG, "Freed 2KB buffer");
        }
        else
        {
            ESP_LOGE(TAG, "Memory allocation failed — Out of memory!");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ===============================
// 3️⃣ Stack Usage Demo Task
// ===============================
void worker_task(void *pv)
{
    char local_buffer[256];
    for (int i = 0; i < sizeof(local_buffer); i++)
        local_buffer[i] = i % 256;

    while (1)
    {
        ESP_LOGI(TAG, "Worker running...");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// ===============================
// Main Application
// ===============================
void app_main(void)
{
    ESP_LOGI(TAG, "=== Exercise 4: Advanced Memory Monitoring ===");

    xTaskCreate(memory_monitor_task, "MemMonitor", 4096, NULL, 5, NULL);
    xTaskCreate(memory_stress_task, "MemStress", 4096, NULL, 4, NULL);
    xTaskCreate(worker_task, "Worker", 2048, NULL, 3, NULL);
}
