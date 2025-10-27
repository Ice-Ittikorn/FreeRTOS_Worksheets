#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EX4_BARRIER";

EventGroupHandle_t xBarrierEventGroup;

// สร้าง bit สำหรับแต่ละ task
#define BIT_TASK_A   BIT0
#define BIT_TASK_B   BIT1
#define BIT_TASK_C   BIT2
#define ALL_READY    (BIT_TASK_A | BIT_TASK_B | BIT_TASK_C)

// ===========================
// Task A
// ===========================
void task_a(void *pv)
{
    while (1)
    {
        // ทำงานบางอย่าง (เวลาแตกต่างกัน)
        vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 2000));
        ESP_LOGI(TAG, "Task A reached barrier");
        xEventGroupSetBits(xBarrierEventGroup, BIT_TASK_A);

        // รอให้ทุกคนพร้อมครบ (Barrier)
        xEventGroupWaitBits(xBarrierEventGroup, ALL_READY, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Task A passing barrier ✅");
    }
}

// ===========================
// Task B
// ===========================
void task_b(void *pv)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1500 + rand() % 2500));
        ESP_LOGI(TAG, "Task B reached barrier");
        xEventGroupSetBits(xBarrierEventGroup, BIT_TASK_B);

        xEventGroupWaitBits(xBarrierEventGroup, ALL_READY, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Task B passing barrier ✅");
    }
}

// ===========================
// Task C
// ===========================
void task_c(void *pv)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1200 + rand() % 3000));
        ESP_LOGI(TAG, "Task C reached barrier");
        xEventGroupSetBits(xBarrierEventGroup, BIT_TASK_C);

        xEventGroupWaitBits(xBarrierEventGroup, ALL_READY, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Task C passing barrier ✅");
    }
}

// ===========================
// Monitor task – แสดงสถานะ event
// ===========================
void monitor_task(void *pv)
{
    while (1)
    {
        EventBits_t bits = xEventGroupGetBits(xBarrierEventGroup);
        ESP_LOGI(TAG, "Barrier bits: [A:%d B:%d C:%d]",
                 (bits & BIT_TASK_A) ? 1 : 0,
                 (bits & BIT_TASK_B) ? 1 : 0,
                 (bits & BIT_TASK_C) ? 1 : 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ===========================
// app_main()
// ===========================
void app_main(void)
{
    srand((unsigned int)xTaskGetTickCount());
    ESP_LOGI(TAG, "Starting Exercise 4: Task Barrier Synchronization");

    // สร้าง Event Group
    xBarrierEventGroup = xEventGroupCreate();

    // สร้าง tasks
    xTaskCreate(task_a, "TaskA", 2048, NULL, 3, NULL);
    xTaskCreate(task_b, "TaskB", 2048, NULL, 3, NULL);
    xTaskCreate(task_c, "TaskC", 2048, NULL, 3, NULL);
    xTaskCreate(monitor_task, "Monitor", 2048, NULL, 2, NULL);
}
