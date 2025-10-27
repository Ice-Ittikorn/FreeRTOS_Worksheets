#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EX3_MULTI_PHASE";

// Event group สำหรับจัดการแต่ละ Phase
EventGroupHandle_t xPhaseEventGroup;

// กำหนด bit สำหรับแต่ละ Phase
#define BIT_PHASE1   BIT0
#define BIT_PHASE2   BIT1
#define BIT_PHASE3   BIT2
#define BIT_PHASE4   BIT3
#define BIT_PHASE5   BIT4

// ===========================
// Phase 1 - Hardware Init
// ===========================
void phase1_task(void *pv)
{
    ESP_LOGI(TAG, "Phase 1: Initializing Hardware...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 2000));
    ESP_LOGI(TAG, "✅ Phase 1 complete (Hardware Ready)");
    xEventGroupSetBits(xPhaseEventGroup, BIT_PHASE1);
    vTaskDelete(NULL);
}

// ===========================
// Phase 2 - Network Init
// ===========================
void phase2_task(void *pv)
{
    // รอให้ Phase 1 เสร็จก่อน
    xEventGroupWaitBits(xPhaseEventGroup, BIT_PHASE1, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Phase 2: Initializing Network...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 2000));
    ESP_LOGI(TAG, "✅ Phase 2 complete (Network Ready)");
    xEventGroupSetBits(xPhaseEventGroup, BIT_PHASE2);
    vTaskDelete(NULL);
}

// ===========================
// Phase 3 - Configuration Load
// ===========================
void phase3_task(void *pv)
{
    xEventGroupWaitBits(xPhaseEventGroup, BIT_PHASE2, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Phase 3: Loading Configuration...");
    vTaskDelay(pdMS_TO_TICKS(1000 + rand() % 2000));
    ESP_LOGI(TAG, "✅ Phase 3 complete (Config Loaded)");
    xEventGroupSetBits(xPhaseEventGroup, BIT_PHASE3);
    vTaskDelete(NULL);
}

// ===========================
// Phase 4 - Sensor Calibration
// ===========================
void phase4_task(void *pv)
{
    xEventGroupWaitBits(xPhaseEventGroup, BIT_PHASE3, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Phase 4: Calibrating Sensors...");
    vTaskDelay(pdMS_TO_TICKS(1500 + rand() % 2000));
    ESP_LOGI(TAG, "✅ Phase 4 complete (Sensors Calibrated)");
    xEventGroupSetBits(xPhaseEventGroup, BIT_PHASE4);
    vTaskDelete(NULL);
}

// ===========================
// Phase 5 - Application Start
// ===========================
void phase5_task(void *pv)
{
    xEventGroupWaitBits(xPhaseEventGroup, BIT_PHASE4, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Phase 5: Starting Application...");
    vTaskDelay(pdMS_TO_TICKS(1500));
    ESP_LOGI(TAG, "🚀 System fully operational!");
    xEventGroupSetBits(xPhaseEventGroup, BIT_PHASE5);
    vTaskDelete(NULL);
}

// ===========================
// Monitor Task - แสดงสถานะระบบ
// ===========================
void monitor_task(void *pv)
{
    while (1)
    {
        EventBits_t bits = xEventGroupGetBits(xPhaseEventGroup);
        ESP_LOGI(TAG, "System Status: [P1:%d P2:%d P3:%d P4:%d P5:%d]",
                 (bits & BIT_PHASE1) ? 1 : 0,
                 (bits & BIT_PHASE2) ? 1 : 0,
                 (bits & BIT_PHASE3) ? 1 : 0,
                 (bits & BIT_PHASE4) ? 1 : 0,
                 (bits & BIT_PHASE5) ? 1 : 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ===========================
// app_main()
// ===========================
void app_main(void)
{
    srand((unsigned int)xTaskGetTickCount());
    ESP_LOGI(TAG, "Starting Exercise 3: Multi-Phase Startup");

    // สร้าง Event Group
    xPhaseEventGroup = xEventGroupCreate();

    // สร้างแต่ละ Phase
    xTaskCreate(phase1_task, "Phase1", 2048, NULL, 4, NULL);
    xTaskCreate(phase2_task, "Phase2", 2048, NULL, 4, NULL);
    xTaskCreate(phase3_task, "Phase3", 2048, NULL, 4, NULL);
    xTaskCreate(phase4_task, "Phase4", 2048, NULL, 4, NULL);
    xTaskCreate(phase5_task, "Phase5", 2048, NULL, 4, NULL);

    // สร้าง Monitor Task
    xTaskCreate(monitor_task, "Monitor", 2048, NULL, 2, NULL);
}
