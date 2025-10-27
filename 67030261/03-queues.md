# Lab 1: Basic Queue Operations
### คำถามสำหรับการทดลอง
1. เมื่อ Queue เต็ม การเรียก `xQueueSend` จะเกิดอะไรขึ้น?
  - ถ้า Queue เต็ม Task ที่เรียก xQueueSend() จะ รอ (block) จนกว่าจะมีที่ว่าง หรือถ้ากำหนด timeout แล้วหมดเวลา ก็จะส่งไม่สำเร็จและคืนค่า pdFAIL
2. เมื่อ Queue ว่าง การเรียก `xQueueReceive` จะเกิดอะไรขึ้น?
  - ถ้า Queue ว่าง Task ที่เรียก xQueueReceive() จะ รอจนกว่าจะมีข้อความเข้ามา หรือถ้าเกินเวลาที่กำหนดไว้ใน timeout ก็จะรับไม่สำเร็จและคืนค่า pdFAIL
3. ทำไม LED จึงกะพริบตามการส่งและรับข้อความ?
  - เพราะในโค้ดมีการสั่งให้ LED ติดและดับทุกครั้งที่ ส่งหรือรับข้อมูลผ่าน Queue สำเร็จ เพื่อใช้แสดงสถานะการทำงานของ sender และ receiver แต่ละตัวแบบมองเห็นได้ง่าย
  - ตัวอย่าง
    ```
    if (xStatus == pdPASS) {
        ESP_LOGI(TAG, "Sent: ID=%d, MSG=%s, Time=%lu", 
                message.id, message.message, message.timestamp);
        
        // 🔸 ทำให้ LED กะพริบเมื่อส่งข้อมูลสำเร็จ
        gpio_set_level(LED_SENDER, 1);      // เปิดไฟ LED
        vTaskDelay(pdMS_TO_TICKS(100));     // รอ 100 ms
        gpio_set_level(LED_SENDER, 0);      // ปิดไฟ LED
    }
    ```
