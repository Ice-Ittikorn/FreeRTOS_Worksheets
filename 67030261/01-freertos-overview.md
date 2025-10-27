# Lab 1: ESP-IDF Setup และโปรเจกต์แรก
## คำถามทบทวหน

1. ไฟล์ใดบ้างที่จำเป็นสำหรับโปรเจกต์ ESP-IDF ขั้นต่ำ?
   - CMakeLists.txt (ทั้งใน root และ main)
   - main/main.c (หรือไฟล์ C/C++ ของแอปหลัก)
   - sdkconfig (เก็บการตั้งค่าโปรเจกต์)
2. ความแตกต่างระหว่าง `hello_esp32.bin` และ `hello_esp32.elf` คืออะไร?
  -   .elf → Executable แบบที่ใช้ debug, มี symbol, ใช้ใน IDE/Debugger
  -   .bin → Binary สำหรับแฟลชลง ESP32, ไม่รวม symbol
3. คำสั่ง `idf.py set-target` ทำอะไร?
  - ตั้งค่า target chip เพื่อคอมไพล์ให้ตรงกับฮาร์ดแวร์
4. โฟลเดอร์ `build/` มีไฟล์อะไรบ้าง?
  - Binary/ELF (`.bin`, `.elf`)
  - Object files (`.o`)
  - Linker scripts (`.ld`)
  - Map file, log build, CMake cache, component libraries
5. การใช้ `vTaskDelay()` แทน `delay()` มีความสำคัญอย่างไร?
  - vTaskDelay() → FreeRTOS-aware, ให้ task อื่นรันได้, ป้องกัน blocking
  - delay() → blocking, หยุดทั้ง CPU, ไม่เหมาะกับ multitasking

# Lab 2: Hello World และ Serial Communication

## คำถามทบทวน
1. ความแตกต่างระหว่าง `printf()` และ `ESP_LOGI()` คืออะไร?   
  - printf() → แสดงข้อความธรรมดา ไม่มี tag, ไม่มี log level, ไม่มีเวลา
  - ESP_LOGI() → เป็น logging ของ ESP-IDF, มี **tag, log level, timestamp**, จัดการง่ายและสามารถกรองได้
2. Log level ไหนที่จะแสดงใน default configuration?
  - `Info` (ESP_LOGI) และสูงกว่า (`Warning`, `Error`)
3. การใช้ `ESP_ERROR_CHECK()` มีประโยชน์อย่างไร?
  - ตรวจสอบค่า return ของฟังก์ชัน ESP-IDF (esp_err_t)
  - ถ้าเกิด error → แสดง log และหยุดโปรแกรมทันที
4. คำสั่งใดในการออกจาก Monitor mode?
  - Ctrl + ]
5. การตั้งค่า Log level สำหรับ tag เฉพาะทำอย่างไร?
  - ใช้ `esp_log_level_set("TAG_NAME", ESP_LOG_LEVEL_INFO);`
  - สามารถตั้งได้เป็น `NONE`, `ERROR`, `WARN`, `INFO`, `DEBUG`, `VERBOSE`

# Lab 3: สร้าง Task แรกด้วย FreeRTOS

## คำถามทบทวน
1. เหตุใด Task function ต้องมี infinite loop?
   - เพราะใน FreeRTOS โค้ดของ task ถ้า return ไปแล้ว ตัว scheduler ไม่ได้เตรียมจัดการคืนค่าหรือปิด task อัตโนมัติ ดังนั้นจึงต้องอยู่ใน loop ตลอด เพื่อไม่ให้ task จบหน้าที่โดยไม่ตั้งใจ 
2. ความหมายของ stack size ใน xTaskCreate() คืออะไร?
   - คือขนาดของ stack memory ที่แยกให้กับ task นั้นใช้ (รวมฟังก์ชัน, ตัวแปรภายใน task) ถ้าให้เล็กเกินไป อาจเกิด stack overflow ได้
3. ความแตกต่างระหว่าง vTaskDelay() และ vTaskDelayUntil()?
   - vTaskDelay() บอกให้ task บล็อกไปเป็นเวลาที่กำหนด จากเวลาปัจจุบัน 
   - vTaskDelayUntil() บอกให้ task บล็อกจนถึง เวลาที่กำหนดแบบ absolute เพื่อให้ task ทำงานแบบ period ที่คงที่
4. การใช้ vTaskDelete(NULL) vs vTaskDelete(handle) ต่างกันอย่างไร?
   - vTaskDelete(NULL) จะลบ (terminate) task ที่เรียกใช้งานอยู่ตอนนั้น
   - vTaskDelete(handle) จะลบ task ที่มี handle ระบุไว้ (อาจเป็น task อื่น)
5. Priority 0 กับ Priority 24 อันไหนสูงกว่า?
   - ค่า priority ที่ “สูงกว่า” คือมีตัวเลข มากกว่า ดังนั้น Priority 24 สูงกว่า Priority 0
