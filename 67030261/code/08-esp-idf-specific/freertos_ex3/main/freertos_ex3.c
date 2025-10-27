#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "SYNC_EX3";

// ---------------------------
// Global Semaphores
// ---------------------------
SemaphoreHandle_t sensor_ready_sem;
SemaphoreHandle_t control_done_sem;

// ---------------------------
// Sensor Task  (Core 0)
// ---------------------------
void sensor_task(void *param)
{
    const TickType_t delay = pdMS_TO_TICKS(1000);  // every 1s

    while (1)
    {
        ESP_LOGI(TAG, "Sensor: collecting data...");
        vTaskDelay(delay);

        // Signal Control task
        xSemaphoreGive(sensor_ready_sem);
        ESP_LOGI(TAG, "Sensor ready -> Control start");
    }
}

// ---------------------------
// Control Task  (Core 0)
// ---------------------------
void control_task(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(sensor_ready_sem, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "Control: processing data...");
            vTaskDelay(pdMS_TO_TICKS(500));  // simulate work

            // Signal Logger
            xSemaphoreGive(control_done_sem);
            ESP_LOGI(TAG, "Control done -> Logger updated");
        }
    }
}

// ---------------------------
// Logger Task  (Core 1)
// ---------------------------
void logger_task(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(control_done_sem, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "Logger: data stored successfully.");
        }
    }
}

// ---------------------------
// app_main()
// ---------------------------
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Exercise 3: Inter-Task Synchronization");

    // Create semaphores
    sensor_ready_sem = xSemaphoreCreateBinary();
    control_done_sem = xSemaphoreCreateBinary();

    // Create tasks
    xTaskCreatePinnedToCore(sensor_task, "SensorTask", 4096, NULL, 8, NULL, 0);
    xTaskCreatePinnedToCore(control_task, "ControlTask", 4096, NULL, 9, NULL, 0);
    xTaskCreatePinnedToCore(logger_task, "LoggerTask", 4096, NULL, 5, NULL, 1);
}
