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
