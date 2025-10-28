# Lab 1: Binary Semaphores 
### คำถามสำหรับการทดลоง
1. เมื่อ give semaphore หลายครั้งติดต่อกัน จะเกิดอะไรขึ้น?
  - Binary Semaphore มีค่าได้เพียงสองสถานะคือ 0 (empty) หรือ 1 (available)เมื่อมีการ xSemaphoreGive() หลายครั้งติดต่อกันโดยยังไม่มี task ไหน take ไป ค่า semaphore จะ ไม่เกิน 1ดังนั้น การ give ซ้ำ ๆ จะไม่มีผลเพิ่มเติม หลังจากครั้งแรก — ค่า count จะคงที่ที่ 1
2. ISR สามารถใช้ `xSemaphoreGive` หรือต้องใช้ `xSemaphoreGiveFromISR`?
  - ใน ISR ต้องใช้ xSemaphoreGiveFromISR() เพราะฟังก์ชันนี้ถูกออกแบบมาให้ ปลอดภัยต่อการใช้งานใน interrupt context — มันจะไม่เรียกการสลับ task โดยตรง แต่จะตั้งค่าสถานะให้ scheduler จัดการหลัง ISR จบ ซึ่งต่างจาก xSemaphoreGive() ที่ออกแบบมาให้ใช้ใน task context เท่านั้น
3. Binary Semaphore แตกต่างจาก Queue อย่างไร?
  - วัตถุประสงค์การใช้งานต่างกัน
    - Binary Semaphore ใช้สำหรับ “การซิงโครไนซ์เหตุการณ์” เช่น บอกว่าเหตุการณ์หนึ่งเกิดขึ้นแล้ว
    - Queue ใช้สำหรับ “การส่งข้อมูล” ระหว่าง task หนึ่งไปยังอีก task หนึ่ง
  - ลักษณะของข้อมูลที่เก็บต่างกัน
    - Binary Semaphore ไม่มีข้อมูลในตัว มีเพียงสถานะ 0 หรือ 1
    - Queue มีช่องเก็บข้อมูลจริง สามารถบรรจุข้อมูลหลายชิ้นตามขนาดที่กำหนดได้
  - จำนวนค่าที่เก็บได้ต่างกัน
    - Binary Semaphore เก็บได้แค่ 1 ครั้ง (ให้แล้วต้องถูก take ก่อนถึงจะให้ใหม่ได้)
    - Queue สามารถเก็บได้หลายรายการแบบคิว FIFO (มาก่อนได้ก่อน)
  - ลักษณะการใช้งานต่างกัน
    - Binary Semaphore ใช้เพื่อแจ้งเหตุการณ์ เช่น ISR ให้สัญญาณให้ task ทำงานต่อ
    - Queue ใช้เพื่อส่งค่าหรือข้อมูลจริง เช่น ส่งค่าเซนเซอร์หรือข้อความระหว่าง task
  - แนวคิดพื้นฐานต่างกัน
    - Binary Semaphore = “สัญญาณ”
    - Queue = “กล่องใส่ข้อมูล”

# Lab 2: Mutex and Critical Sections 
### คำถามสำหรับการทดลอง
1. เมื่อไม่ใช้ Mutex จะเกิด data corruption หรือไม่?
  - เกิด เพราะหลาย Task เข้าถึงและแก้ไข shared resource พร้อมกัน ทำให้ข้อมูลเขียนทับกันหรือผิดพลาด (race condition)
2. Priority Inheritance ทำงานอย่างไร?
  - ระบบจะเพิ่ม Priority ชั่วคราวให้ Task ที่ถือ Mutex อยู่ เพื่อให้มันทำงานเสร็จและคืน Mutex ได้เร็ว ป้องกันปัญหา Priority Inversion
3. Task priority มีผลต่อการเข้าถึง shared resource อย่างไร?
  - Task ที่มี Priority สูงจะได้ CPU ก่อน จึงมีโอกาสเข้าถึง resource ก่อน แต่ถ้า resource ถูกล็อกโดย Task ต่ำ ระบบจะใช้ Priority Inheritance มาช่วยให้ไม่เกิดการรอค้าง

# Lab 3: Counting Semaphores
### คำถามสำหรับการทดลอง
1. เมื่อ Producers มากกว่า Resources จะเกิดอะไรขึ้น?
  - Producers จะต้อง รอคิว (blocked) เพื่อขอใช้ resource ผ่าน xSemaphoreTake()
  - ระบบจะมีบาง producer ได้ resource ทันที ส่วนที่เหลือจะรอจนกว่าทรัพยากรถูกคืน (xSemaphoreGive())
  - ทำให้เกิด การหน่วงเวลา (wait time) และบางครั้งเกิด timeout หากรอนานเกิน 8 วินาที (ตามโค้ด)
  - Success Rate ลดลง
2. Load Generator มีผลต่อ Success Rate อย่างไร?
  - ทำให้ resource pool ถูกใช้จนหมด (Resource pool exhausted)
  - Producers ที่กำลังรอจะยิ่งรอคิวนานขึ้น และบางรายอาจ timeout
  - หลัง load burst จบ Success Rate จะลดลงชั่วคราว แล้วค่อยกลับมาสูงขึ้นเมื่อ resource ถูกคืน
3. Counting Semaphore จัดการ Resource Pool อย่างไร?
  - สร้างด้วย xSemaphoreCreateCounting(MAX_RESOURCES, MAX_RESOURCES) ทุกครั้งที่ task ต้องการใช้ resource จะเรียก xSemaphoreTake() นับลดลงเมื่อใช้งานเสร็จจะเรียก xSemaphoreGive() → นับเพิ่มกลับถ้าค่าต่ำสุดเป็น 0 หมายถึง resource หมด (ต้องรอหรือ timeout)
