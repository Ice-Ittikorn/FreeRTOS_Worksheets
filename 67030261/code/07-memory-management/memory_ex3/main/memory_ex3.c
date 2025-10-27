#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "MEM_LEAK_EX3";

typedef struct {
    void *ptr;
    size_t size;
    const char *tag;
} allocation_info_t;

#define MAX_TRACKED_ALLOCS 50
allocation_info_t alloc_table[MAX_TRACKED_ALLOCS];

// บันทึกข้อมูลการจองหน่วยความจำ
void* tracked_malloc(size_t size, const char *tag)
{
    void *ptr = pvPortMalloc(size);
    if (ptr == NULL) {
        ESP_LOGE(TAG, "Allocation failed (%s)", tag);
        return NULL;
    }

    for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
        if (alloc_table[i].ptr == NULL) {
            alloc_table[i].ptr = ptr;
            alloc_table[i].size = size;
            alloc_table[i].tag = tag;
            break;
        }
    }

    ESP_LOGI(TAG, "Allocated %d bytes for %s at %p", size, tag, ptr);
    return ptr;
}

// คืนหน่วยความจำและลบออกจากตาราง
void tracked_free(void *ptr)
{
    if (ptr == NULL) return;

    for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
        if (alloc_table[i].ptr == ptr) {
            ESP_LOGI(TAG, "Freed %d bytes from %s at %p", alloc_table[i].size, alloc_table[i].tag, ptr);
            alloc_table[i].ptr = NULL;
            alloc_table[i].size = 0;
            alloc_table[i].tag = NULL;
            vPortFree(ptr);
            return;
        }
    }

    ESP_LOGW(TAG, "Attempted to free untracked pointer %p", ptr);
}

// แสดงหน่วยความจำที่ยังไม่ถูกคืน
void check_memory_leaks(void)
{
    int leaks = 0;
    size_t total_leaked = 0;

    ESP_LOGW(TAG, "=== MEMORY LEAK REPORT ===");
    for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
        if (alloc_table[i].ptr != NULL) {
            ESP_LOGW(TAG, "Leaked: %d bytes from %s at %p", alloc_table[i].size, alloc_table[i].tag, alloc_table[i].ptr);
            total_leaked += alloc_table[i].size;
            leaks++;
        }
    }

    if (leaks == 0)
        ESP_LOGI(TAG, "No memory leaks detected ✅");
    else
        ESP_LOGE(TAG, "Total leaks: %d blocks, %d bytes ⚠️", leaks, total_leaked);
}

// แสดง heap usage ปัจจุบัน
void print_heap_usage(void)
{
    ESP_LOGI(TAG, "Free heap: %d bytes", xPortGetFreeHeapSize());
}

// ==============================
// Task จำลองการรั่วหน่วยความจำ
// ==============================
void memory_leak_demo(void *pv)
{
    void *p1 = tracked_malloc(100, "sensor_buffer");
    void *p2 = tracked_malloc(200, "network_buffer");
    void *p3 = tracked_malloc(150, "temporary_data");

    print_heap_usage();

    // ปล่อยบางอัน
    tracked_free(p2);

    print_heap_usage();

    ESP_LOGI(TAG, "Simulating memory leak... (some memory not freed)");

    vTaskDelay(pdMS_TO_TICKS(5000));
    check_memory_leaks();  // ตรวจจับ leaks

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "=== Exercise 3: Memory Leak Detection ===");
    memset(alloc_table, 0, sizeof(alloc_table));
    xTaskCreate(memory_leak_demo, "LeakDemo", 4096, NULL, 5, NULL);
}
