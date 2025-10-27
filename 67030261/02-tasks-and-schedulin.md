# Lab 1: Task Priority และ Scheduling
## คำถามสำหรับวิเคราะห์
1. Priority ไหนทำงานมากที่สุด? เพราะอะไร?
  - High Priority Task ทำงานมากสุด เพราะ งานที่มี priority สูงที่สุด (ตัวเลขใหญ่กว่า) จะได้รับ CPU ก่อนเสมอ เพราะ FreeRTOS scheduler เลือก task ที่ “Ready” ที่มี priority สูงสุดให้ทำงาน 
2. เกิด Priority Inversion หรือไม่? จะแก้ไขได้อย่างไร?
  - เกิด Priority Inversion
  - ใช้ mutex ที่รองรับ priority inheritance (เมื่อ task ต่ำถือ resource แล้ว task สูงรอ → task ต่ำถูกยก priority ชั่วคราว) หรือออกแบบให้ resource shared น้อยและ critical section สั้น
3. Tasks ที่มี priority เดียวกันทำงานอย่างไร?
  - Tasks ที่มี priority เท่ากันจะไม่ได้แย่ง CPU กัน แต่จะผลัดกันทำงานทีละตัวแบบ time-sharing (round-robin) เพื่อให้ทุกตัวมีโอกาสรันเท่า ๆ กัน.
4. การเปลี่ยน Priority แบบ dynamic ส่งผลอย่างไร?
  - การเปลี่ยน priority แบบ dynamic ทำให้ลำดับการทำงานของ task เปลี่ยนได้ทันที และมีผลต่อการจัดสรร CPU ทันที
5. CPU utilization ของแต่ละ priority เป็นอย่างไร?
  - ยิ่ง priority สูง ยิ่งได้ใช้ CPU มาก เพราะ scheduler ให้รันก่อนเสมอ ส่วน priority ต่ำจะได้รันน้อยลงหรือรอจนกว่า task ที่สูงกว่าจะว่าง

# Lab 2: Task States Demonstration
## คำถามสำหรับวิเคราะห์
1. Task อยู่ใน Running state เมื่อไหร่บ้าง?
  - เมื่อ task นั้นได้รับ CPU จาก scheduler และกำลังประมวลผลจริง เช่นใน loop ของ state_demo_task ตอน LED GPIO2 สว่าง
2. ความแตกต่างระหว่าง Ready และ Blocked state คืออะไร?
  - Ready: task พร้อมทำงาน แต่ยังไม่ได้รับ CPU (รอคิว)
  - Blocked: task รอ event หรือ resource เช่น semaphore หรือ delay
3. การใช้ vTaskDelay() ทำให้ task อยู่ใน state ใด?
  - vTaskDelay() จะทำให้ task เข้าสู่ Blocked state ชั่วคราว จนเวลาที่กำหนดหมด แล้วกลับไป Ready
4. การ Suspend task ต่างจาก Block อย่างไร?
  - Suspend: หยุด task ด้วยคำสั่งจากภายนอก (vTaskSuspend() / Resume) จะไม่กลับมาทำงานจนกว่าจะ vTaskResume()
  - Block: task หยุดเองเพราะรอ event/time แล้วกลับมาอัตโนมัติเมื่อเงื่อนไขครบ
5. Task ที่ถูก Delete จะกลับมาได้หรือไม่?
  - ไม่ได้ Task ที่ถูก vTaskDelete() แล้วจะถูกลบออกจากระบบเลย ต้องสร้างใหม่ด้วย xTaskCreate() เท่านั้น

# Lab 3: Stack Monitoring และ Debugging
## คำถามสำหรับวิเคราะห์
1. Task ไหนใช้ stack มากที่สุด? เพราะอะไร?
  - Heavy Task ใช้ stack มากที่สุด เพราะในโค้ด heavy_stack_task() มีการประกาศ local arrays ขนาดใหญ่  เช่น ```char large_buffer[1024];``` ซึ่งทั้งหมดถูกเก็บใน stack ของ task ทำให้ stack space ถูกใช้จำนวนมากและอาจเกิด warning หรือ overflow ได้
2. การใช้ heap แทน stack มีข้อดีอย่างไร?
  - ป้องกัน stack overflow สำหรับข้อมูลขนาดใหญ่
  - ใช้หน่วยความจำอย่างยืดหยุ่น สามารถคืนพื้นที่ได้ด้วย free()
  - เหมาะกับข้อมูลที่ต้องอยู่ข้ามการเรียกฟังก์ชัน
3. Stack overflow เกิดขึ้นเมื่อไหร่และทำอย่างไรป้องกัน?
  - เกิดเมื่อ task ใช้ stack เกินขนาดที่กำหนดไว้ เช่น การใช้ local array ใหญ่หรือ recursion ลึกเกินไป
  - การป้องกัน:
    - กำหนดค่า stack size ให้พอเพียงกับงานของแต่ละ task
    - เปิดใช้ฟีเจอร์ตรวจสอบ overflow:
    - หลีกเลี่ยงการใช้ตัวแปรขนาดใหญ่ในฟังก์ชัน
    - ใช้ heap สำหรับข้อมูลขนาดใหญ่แทน
4. การตั้งค่า stack size ควรพิจารณาจากอะไร?
    - จำนวนและขนาดของ local variables
    - ความลึกของ function call หรือ recursion
    - ฟังก์ชันที่ใช้ เช่น printf() ใช้ stack เยอะ
    - ผลจากการทดลองจริงด้วย uxTaskGetStackHighWaterMark()
5. Recursion ส่งผลต่อ stack usage อย่างไร?
    - จะทำให้ stack ถูกใช้เพิ่มขึ้นทุกครั้งที่เรียกซ้ำเพราะแต่ละ call จะสร้าง stack frame ใหม่ เพื่อเก็บตัวแปรในฟังก์ชันและ return addressจึงทำให้ stack usage เพิ่มขึ้นแบบทวีคูณตามความลึกของ recursionถ้าลึกเกินไปอาจทำให้เกิด stack overflow ได้

