#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "QUEUE_EX4";

// --------------------------------------------------
// Structures
// --------------------------------------------------
typedef struct {
    int id;
    float value;
    int64_t timestamp;
} sensor_data_t;

// --------------------------------------------------
// Global Queues
// --------------------------------------------------
QueueHandle_t sensor_to_control_q;
QueueHandle_t control_to_logger_q;

// --------------------------------------------------
// Sensor Task (Core 0)
// --------------------------------------------------
void sensor_task(void *param)
{
    sensor_data_t data;
    int counter = 0;
    const TickType_t delay = pdMS_TO_TICKS(1000); // 1s

    while (1)
    {
        data.id = counter++;
        data.value = (float)(esp_timer_get_time() % 1000) / 10.0f;
        data.timestamp = esp_timer_get_time();

        xQueueSend(sensor_to_control_q, &data, portMAX_DELAY);
        ESP_LOGI(TAG, "Sensor -> Sent data ID %d (%.2f)", data.id, data.value);

        vTaskDelay(delay);
    }
}

// --------------------------------------------------
// Control Task (Core 0)
// --------------------------------------------------
void control_task(void *param)
{
    sensor_data_t input, output;

    while (1)
    {
        if (xQueueReceive(sensor_to_control_q, &input, portMAX_DELAY))
        {
            // Simulate data processing
            output.id = input.id;
            output.value = input.value * 1.5f;  // Processed value
            output.timestamp = esp_timer_get_time();

            xQueueSend(control_to_logger_q, &output, portMAX_DELAY);
            ESP_LOGI(TAG, "Control -> Processed ID %d -> %.2f", input.id, output.value);
        }
    }
}

// --------------------------------------------------
// Logger Task (Core 1)
// --------------------------------------------------
void logger_task(void *param)
{
    sensor_data_t received;

    while (1)
    {
        if (xQueueReceive(control_to_logger_q, &received, portMAX_DELAY))
        {
            int64_t latency = esp_timer_get_time() - received.timestamp;
            ESP_LOGI(TAG, "Logger <- ID %d: %.2f (latency: %lld µs)",
                     received.id, received.value, latency);
        }
    }
}

// --------------------------------------------------
// app_main()
// --------------------------------------------------
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Exercise 4: Inter-Core Queue Communication");

    // Create Queues
    sensor_to_control_q = xQueueCreate(5, sizeof(sensor_data_t));
    control_to_logger_q = xQueueCreate(5, sizeof(sensor_data_t));

    if (sensor_to_control_q == NULL || control_to_logger_q == NULL)
    {
        ESP_LOGE(TAG, "Queue creation failed!");
        return;
    }

    // Create Tasks
    xTaskCreatePinnedToCore(sensor_task, "SensorTask", 4096, NULL, 8, NULL, 0);
    xTaskCreatePinnedToCore(control_task, "ControlTask", 4096, NULL, 9, NULL, 0);
    xTaskCreatePinnedToCore(logger_task, "LoggerTask", 4096, NULL, 5, NULL, 1);
}
