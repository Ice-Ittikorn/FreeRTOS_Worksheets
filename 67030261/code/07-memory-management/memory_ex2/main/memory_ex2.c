#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "MEM_POOL_EX2";

// ==========================
// Memory Pool Struct
// ==========================
typedef struct {
    void *pool_start;
    size_t block_size;
    size_t num_blocks;
    uint8_t *alloc_map;
    SemaphoreHandle_t mutex;
} memory_pool_t;

// ==========================
// Create Pool
// ==========================
memory_pool_t* create_pool(size_t block_size, size_t num_blocks)
{
    memory_pool_t *pool = pvPortMalloc(sizeof(memory_pool_t));
    if (!pool) return NULL;

    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->pool_start = pvPortMalloc(block_size * num_blocks);
    pool->alloc_map = pvPortMalloc(num_blocks);
    pool->mutex = xSemaphoreCreateMutex();

    if (!pool->pool_start || !pool->alloc_map || !pool->mutex) {
        ESP_LOGE(TAG, "Pool creation failed!");
        return NULL;
    }

    memset(pool->alloc_map, 0, num_blocks);
    ESP_LOGI(TAG, "Pool created: %d blocks of %d bytes", num_blocks, block_size);
    return pool;
}

// ==========================
// Allocate from Pool
// ==========================
void* pool_malloc(memory_pool_t *pool)
{
    if (!pool) return NULL;
    if (xSemaphoreTake(pool->mutex, pdMS_TO_TICKS(100)) != pdTRUE) return NULL;

    for (size_t i = 0; i < pool->num_blocks; i++) {
        if (pool->alloc_map[i] == 0) {
            pool->alloc_map[i] = 1;
            void *block = (uint8_t*)pool->pool_start + (i * pool->block_size);
            xSemaphoreGive(pool->mutex);
            ESP_LOGI(TAG, "Allocated block %d at %p", i, block);
            return block;
        }
    }

    xSemaphoreGive(pool->mutex);
    ESP_LOGW(TAG, "Pool full, allocation failed");
    return NULL;
}

// ==========================
// Free to Pool
// ==========================
void pool_free(memory_pool_t *pool, void *ptr)
{
    if (!pool || !ptr) return;
    if (xSemaphoreTake(pool->mutex, pdMS_TO_TICKS(100)) != pdTRUE) return;

    size_t index = ((uint8_t*)ptr - (uint8_t*)pool->pool_start) / pool->block_size;
    if (index < pool->num_blocks && pool->alloc_map[index] == 1) {
        pool->alloc_map[index] = 0;
        ESP_LOGI(TAG, "Freed block %d (%p)", index, ptr);
    } else {
        ESP_LOGW(TAG, "Invalid free operation at %p", ptr);
    }

    xSemaphoreGive(pool->mutex);
}

// ==========================
// Show Pool Status
// ==========================
void pool_status(memory_pool_t *pool)
{
    if (!pool) return;

    size_t used = 0;
    for (size_t i = 0; i < pool->num_blocks; i++) {
        if (pool->alloc_map[i]) used++;
    }

    ESP_LOGI(TAG, "Pool status: %d/%d blocks used (%.1f%%)",
             used, pool->num_blocks,
             (float)used * 100 / pool->num_blocks);
}

// ==========================
// Demo Task
// ==========================
void pool_demo_task(void *pv)
{
    memory_pool_t *pool = create_pool(64, 10); // 10 blocks, 64 bytes each
    void *blocks[5];

    // Allocate 5 blocks
    for (int i = 0; i < 5; i++) {
        blocks[i] = pool_malloc(pool);
        if (blocks[i])
            sprintf((char*)blocks[i], "Block %d data", i);
    }

    pool_status(pool);

    // Free 2 blocks
    pool_free(pool, blocks[1]);
    pool_free(pool, blocks[3]);

    pool_status(pool);

    // Allocate again to reuse freed blocks
    void *new_block = pool_malloc(pool);
    if (new_block) {
        strcpy((char*)new_block, "Reused Block");
    }

    pool_status(pool);

    vTaskDelete(NULL);
}

// ==========================
// app_main()
// ==========================
void app_main(void)
{
    ESP_LOGI(TAG, "=== Exercise 2: Memory Pool Implementation ===");
    xTaskCreate(pool_demo_task, "PoolDemo", 4096, NULL, 5, NULL);
}
