#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EX1_EVENT_GROUP";

EventGroupHandle_t xInitEventGroup;

// สร้าง bit สำหรับแต่ละ task
#define BIT_HARDWARE_INIT   BIT0
#define BIT_NETWORK_INIT    BIT1
#define BIT_CONFIG_INIT     BIT2

// ===========================
// Task 1: Hardware Initialization
// ===========================
void hardware_task(void *pv)
{
    ESP_LOGI(TAG, "Hardware initializing...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 4000)); // delay 1–5 วินาที
    ESP_LOGI(TAG, "Hardware ready ✅");
    xEventGroupSetBits(xInitEventGroup, BIT_HARDWARE_INIT);
    vTaskDelete(NULL);
}

// ===========================
// Task 2: Network Initialization
// ===========================
void network_task(void *pv)
{
    ESP_LOGI(TAG, "Network initializing...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 4000));
    ESP_LOGI(TAG, "Network ready ✅");
    xEventGroupSetBits(xInitEventGroup, BIT_NETWORK_INIT);
    vTaskDelete(NULL);
}

// ===========================
// Task 3: Config Initialization
// ===========================
void config_task(void *pv)
{
    ESP_LOGI(TAG, "Loading configuration...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 4000));
    ESP_LOGI(TAG, "Config loaded ✅");
    xEventGroupSetBits(xInitEventGroup, BIT_CONFIG_INIT);
    vTaskDelete(NULL);
}

// ===========================
// Main Task
// ===========================
void main_task(void *pv)
{
    ESP_LOGI(TAG, "Main task waiting for system initialization...");

    EventBits_t bits = xEventGroupWaitBits(
        xInitEventGroup,
        BIT_HARDWARE_INIT | BIT_NETWORK_INIT | BIT_CONFIG_INIT, // bits ที่ต้องรอ
        pdTRUE,   // clear เมื่อรับแล้ว
        pdTRUE,   // ต้องครบทุก bit
        pdMS_TO_TICKS(10000) // timeout 10 วินาที
    );

    if ((bits & (BIT_HARDWARE_INIT | BIT_NETWORK_INIT | BIT_CONFIG_INIT)) ==
        (BIT_HARDWARE_INIT | BIT_NETWORK_INIT | BIT_CONFIG_INIT))
    {
        ESP_LOGI(TAG, "✅ All systems initialized successfully!");
    }
    else
    {
        ESP_LOGW(TAG, "⚠️ Initialization timeout! Running in safe mode...");
    }

    vTaskDelete(NULL);
}

// ===========================
// app_main()
// ===========================
void app_main(void)
{
    srand((unsigned int)xTaskGetTickCount());
    ESP_LOGI(TAG, "Starting Exercise 1: Basic Event Coordination");

    // สร้าง Event Group
    xInitEventGroup = xEventGroupCreate();

    // สร้าง Tasks
    xTaskCreate(hardware_task, "HardwareTask", 2048, NULL, 3, NULL);
    xTaskCreate(network_task, "NetworkTask", 2048, NULL, 3, NULL);
    xTaskCreate(config_task, "ConfigTask", 2048, NULL, 3, NULL);
    xTaskCreate(main_task, "MainTask", 2048, NULL, 2, NULL);
}
