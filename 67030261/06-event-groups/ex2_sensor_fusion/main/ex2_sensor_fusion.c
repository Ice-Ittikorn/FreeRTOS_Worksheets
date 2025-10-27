#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EX2_SENSOR_FUSION";

// สร้าง Event Group
EventGroupHandle_t xSensorEventGroup;

// สร้าง Bit ของแต่ละ sensor
#define BIT_TEMP_READY   BIT0
#define BIT_HUMID_READY  BIT1
#define BIT_PRESS_READY  BIT2

// ===========================
// Temperature Sensor
// ===========================
void temp_sensor_task(void *pv)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000)); // อ่านทุก 2 วินาที
        ESP_LOGI(TAG, "🌡️ Temperature ready: 25°C");
        xEventGroupSetBits(xSensorEventGroup, BIT_TEMP_READY);
    }
}

// ===========================
// Humidity Sensor
// ===========================
void humid_sensor_task(void *pv)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2500)); // อ่านทุก 2.5 วินาที
        ESP_LOGI(TAG, "💧 Humidity ready: 60%%");
        xEventGroupSetBits(xSensorEventGroup, BIT_HUMID_READY);
    }
}

// ===========================
// Pressure Sensor
// ===========================
void press_sensor_task(void *pv)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000)); // อ่านทุก 3 วินาที
        ESP_LOGI(TAG, "🌬️ Pressure ready: 1013 hPa");
        xEventGroupSetBits(xSensorEventGroup, BIT_PRESS_READY);
    }
}

// ===========================
// Fusion Task
// ===========================
void fusion_task(void *pv)
{
    while (1)
    {
        // รอให้ Temp + Humidity พร้อมก่อนทำ Basic Fusion
        EventBits_t bits = xEventGroupWaitBits(
            xSensorEventGroup,
            BIT_TEMP_READY | BIT_HUMID_READY,
            pdTRUE,   // clear เมื่ออ่านแล้ว
            pdTRUE,   // ต้องครบทั้งสอง bit
            pdMS_TO_TICKS(5000)
        );

        if ((bits & (BIT_TEMP_READY | BIT_HUMID_READY)) ==
            (BIT_TEMP_READY | BIT_HUMID_READY))
        {
            ESP_LOGI(TAG, "🧪 Basic Fusion: Comfort Index Calculated ✅");
        }

        // รอให้ Temp + Humid + Pressure พร้อมก่อนทำ Advanced Fusion
        bits = xEventGroupWaitBits(
            xSensorEventGroup,
            BIT_TEMP_READY | BIT_HUMID_READY | BIT_PRESS_READY,
            pdTRUE,
            pdTRUE,
            pdMS_TO_TICKS(8000)
        );

        if ((bits & (BIT_TEMP_READY | BIT_HUMID_READY | BIT_PRESS_READY)) ==
            (BIT_TEMP_READY | BIT_HUMID_READY | BIT_PRESS_READY))
        {
            ESP_LOGI(TAG, "⚙️ Advanced Fusion: Environmental Index Calculated ✅");
        }
    }
}

// ===========================
// Main Function
// ===========================
void app_main(void)
{
    ESP_LOGI(TAG, "Starting Exercise 2: Sensor Data Fusion");

    // สร้าง Event Group
    xSensorEventGroup = xEventGroupCreate();

    // สร้าง tasks ของแต่ละ sensor
    xTaskCreate(temp_sensor_task, "TempSensor", 2048, NULL, 2, NULL);
    xTaskCreate(humid_sensor_task, "HumidSensor", 2048, NULL, 2, NULL);
    xTaskCreate(press_sensor_task, "PressSensor", 2048, NULL, 2, NULL);

    // สร้าง fusion task
    xTaskCreate(fusion_task, "FusionTask", 2048, NULL, 3, NULL);
}
