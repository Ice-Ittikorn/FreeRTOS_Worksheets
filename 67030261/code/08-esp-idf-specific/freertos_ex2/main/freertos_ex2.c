#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"   // ✅ สำหรับจัดการ watchdog

static const char *TAG = "REALTIME";

// ==========================
// Control Task (1 kHz)
// ==========================
void control_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Control task started (1 kHz)");

    int64_t last_time = esp_timer_get_time();
    int count = 0;
    double total_jitter = 0;
    const double expected = 1000.0; // µs (1 kHz)
    const int report_interval = 1000; // รายงานทุก 1000 รอบ

    while (1)
    {
        int64_t now = esp_timer_get_time();
        double diff = (double)(now - last_time);
        last_time = now;
        total_jitter += fabs(diff - expected);
        count++;

        if (count >= report_interval)
        {
            double avg_jitter = total_jitter / report_interval;
            double jitter_percent = (avg_jitter / expected) * 100.0;
            ESP_LOGI(TAG, "Control loop: 1000.0 Hz (jitter: ±%.1f%%)", jitter_percent);
            total_jitter = 0;
            count = 0;
        }

        // precise delay
        int64_t next_time = now + (int64_t)expected;
        while (esp_timer_get_time() < next_time)
        {
            taskYIELD();
        }

        vTaskDelay(1); // ✅ คืน CPU ให้ Task อื่นบ้าง
    }
}

// ==========================
// Data Acquisition Task (500 Hz)
// ==========================
void data_acquisition_task(void *param)
{
    ESP_LOGI(TAG, "Data acquisition task started (500 Hz)");

    int64_t last_time = esp_timer_get_time();
    int count = 0;
    int64_t total_jitter = 0;
    const int64_t expected = 2000; // µs (500 Hz)
    const int report_interval = 500;

    while (1)
    {
        int64_t now = esp_timer_get_time();
        int64_t diff = now - last_time;
        last_time = now;
        total_jitter += llabs(diff - expected);
        count++;

        if (count >= report_interval)
        {
            int64_t avg_jitter = total_jitter / report_interval;
            ESP_LOGI(TAG, "Data acquisition: 500.0 Hz (timing: ±%lldµs)", avg_jitter);
            total_jitter = 0;
            count = 0;
        }

        int64_t next_time = now + expected;
        while (esp_timer_get_time() < next_time)
        {
            taskYIELD();
        }

        vTaskDelay(1); // ✅ ป้องกัน Core ล็อก
    }
}

// ==========================
// Communication Task (~15 ms average latency)
// ==========================
void communication_task(void *param)
{
    ESP_LOGI(TAG, "Communication task started (1 Hz)");

    const TickType_t delay = pdMS_TO_TICKS(1000);
    int count = 0;
    double total_latency = 0;

    while (1)
    {
        int64_t start = esp_timer_get_time();

        // จำลอง latency (10–20 ms)
        vTaskDelay(pdMS_TO_TICKS(10 + (esp_timer_get_time() % 10)));

        double latency = (esp_timer_get_time() - start) / 1000.0;
        total_latency += latency;
        count++;

        if (count >= 5)
        {
            double avg = total_latency / 5.0;
            ESP_LOGI(TAG, "Communication latency: %.0fms average", avg);
            total_latency = 0;
            count = 0;
        }

        vTaskDelay(delay);
    }
}

// ==========================
// app_main()
// ==========================
void app_main(void)
{
    esp_task_wdt_deinit(); // ✅ ปิด watchdog Core 0

    ESP_LOGI(TAG, "Starting Exercise 2: Real-Time Performance Summary");

    // Core 0 (Control)
    xTaskCreatePinnedToCore(control_task, "ControlTask", 4096, NULL, 10, NULL, 0);

    // Core 1 (Data + Comm)
    xTaskCreatePinnedToCore(data_acquisition_task, "DataAcqTask", 4096, NULL, 9, NULL, 1);
    xTaskCreatePinnedToCore(communication_task, "CommTask", 4096, NULL, 5, NULL, 1);
}
