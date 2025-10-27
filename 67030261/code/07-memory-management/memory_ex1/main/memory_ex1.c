#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "MEM_EX1";

// ========== STATIC ALLOCATION ==========
#define STATIC_STACK_SIZE 2048
static StackType_t static_task_stack[STATIC_STACK_SIZE];
static StaticTask_t static_task_tcb;

static uint8_t static_queue_storage[10 * sizeof(int)];
static StaticQueue_t static_queue_struct;

void static_task(void *pv)
{
    int count = 0;
    while (1)
    {
        ESP_LOGI(TAG, "Static Task running %d", count++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ========== DYNAMIC ALLOCATION ==========
void dynamic_task(void *pv)
{
    int count = 0;
    while (1)
    {
        ESP_LOGI(TAG, "Dynamic Task running %d", count++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ========== MAIN APP ==========
void app_main(void)
{
    ESP_LOGI(TAG, "=== Exercise 1: Memory Allocation Comparison ===");

    ESP_LOGI(TAG, "Free heap before static allocation: %d bytes", xPortGetFreeHeapSize());

    // สร้าง Static Task
    TaskHandle_t static_handle = xTaskCreateStatic(
        static_task,
        "StaticTask",
        STATIC_STACK_SIZE,
        NULL,
        5,
        static_task_stack,
        &static_task_tcb
    );

    // สร้าง Static Queue
    QueueHandle_t static_queue = xQueueCreateStatic(
        10,
        sizeof(int),
        static_queue_storage,
        &static_queue_struct
    );

    ESP_LOGI(TAG, "Static task created at %p", static_handle);
    ESP_LOGI(TAG, "Free heap after static allocation: %d bytes", xPortGetFreeHeapSize());

    // สร้าง Dynamic Task
    ESP_LOGI(TAG, "Creating dynamic task...");
    xTaskCreate(dynamic_task, "DynamicTask", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "Free heap after dynamic task: %d bytes", xPortGetFreeHeapSize());

    // สร้าง Dynamic Queue
    QueueHandle_t dynamic_queue = xQueueCreate(10, sizeof(int));
    ESP_LOGI(TAG, "Free heap after dynamic queue: %d bytes", xPortGetFreeHeapSize());
}
