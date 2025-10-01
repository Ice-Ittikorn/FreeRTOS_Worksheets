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
