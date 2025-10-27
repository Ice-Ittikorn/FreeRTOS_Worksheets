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
# Lab 2: Producer-Consumer System 
### คำถามสำหรับการทดลอง
1. ในทดลองที่ 2 เกิดอะไรขึ้นกับ Queue?
  - เมื่อเพิ่ม ผู้ผลิต (Producers) จาก 3 เป็น 4 แต่ยังคงมีผู้บริโภค (Consumers) เพียง 2 ตัว ผลิตสินค้าเร็วกว่าอัตราการบริโภค
  - Queue จะเต็มบ่อยมาก (xQueueSend ส่งไม่ทัน → มีการ drop ข้อมูล)
  - ใน Serial Monitor จะเห็นข้อความ
  ```
  ✗ Producer X: Queue full! Dropped Product-PX-#Y
  ⚠️ HIGH LOAD DETECTED! Queue size: 10
  ```
  - แสดงว่าระบบเกิด “Queue Overflow”และ Load Balancer จะแจ้งเตือนว่าควรเพิ่ม Consumer หรือเพิ่ม Buffer
2. ในทดลองที่ 3 ระบบทำงานเป็นอย่างไร?
  - ลดจำนวน ผู้บริโภค (Consumers) จาก 2 เหลือ 1
  - ทำให้ Consumer ทำงานช้ากว่าการผลิต → Queue สะสมข้อมูลมากขึ้นเรื่อย ๆ
  - สุดท้าย Queue จะเต็มบ่อย → มี Dropped Products เพิ่มขึ้นมาก
  - Efficiency ลดลงต่ำ (~59%) เพราะสินค้าที่ผลิตถูก drop ไปก่อนถูกใช้
  - ระบบทำงานช้าลงโดยรวม (เพราะ consumer เดียวต้องประมวลผลทั้งหมด)
3. Load Balancer แจ้งเตือนเมื่อไหร่?
  - Queue มีข้อมูล มากกว่า 8 รายการ

# Lab 3: Queue Sets Implementation (30 นาที)
### คำถามสำหรับการทดลอง
1. Processor Task รู้ได้อย่างไรว่าข้อมูลมาจาก Queue ไหน?
  - Queue ไหนถูกกระตุ้นก่อน (activated) ใน Queue Set ระบบจะส่ง handle ของ Queue นั้นกลับมา ทำให้รู้ทันทีว่าข้อมูลมาจากไหน
2. เมื่อหลาย Queue มีข้อมูลพร้อมกัน เลือกประมวลผลอันไหนก่อน?
  - จะเลือกคิวที่ถูกเพิ่มเข้า Queue Set ก่อนเป็นอันดับแรก (ตามลำดับที่ประกาศไว้)
3. Queue Sets ช่วยประหยัด CPU อย่างไร?
  - ไม่ต้องวนเช็กทุกคิว (polling) แต่รอให้มีข้อมูลจากคิวใดคิวหนึ่งก่อนค่อยทำงาน ทำให้ CPU ว่างและประหยัดพลังงานมากขึ้น
