# Lab 2: Timer Applications - Watchdog & LED Patterns 
## 📋 Post-Lab Questions
1. **Watchdog Design**: เหตุใดต้องใช้ separate timer สำหรับ feeding watchdog?
  - จำเป็นต้องใช้ timer แยก (feed_timer) สำหรับ “feeding watchdog” เพราะหน้าที่ของมันคือ บอกว่า system ยังทำงานอยู่ปกติ โดยไม่ขึ้นกับการทำงานของส่วนอื่นในระบบ เช่น LED pattern หรือ sensor tasks
2. **Pattern Timing**: อธิบายการเลือก Timer Period สำหรับแต่ละ pattern
  - Pattern Timing: การเลือก Timer Period ของแต่ละ pattern จะพิจารณาจาก “วัตถุประสงค์และความเร่งด่วนของสัญญาณ” เช่น
    - Slow Blink (1000 ms) ใช้จังหวะช้าเพื่อบอกสถานะปกติของระบบ ผู้ใช้สามารถมองเห็นได้โดยไม่รบกวนสายตา
    - Fast Blink (200 ms) ใช้จังหวะเร็วเพื่อสื่อถึงเหตุฉุกเฉินหรือการแจ้งเตือนที่ต้องการความสนใจทันที
    - Heartbeat (100 ms) ใช้จังหวะสั้นซ้ำสองครั้ง คล้ายการเต้นหัวใจ สื่อว่าระบบยังทำงานอยู่
    - SOS (200–600 ms) ใช้จังหวะตามรหัสมอร์ส จุด (สั้น) และขีด (ยาว) เพื่อส่งสัญญาณขอความช่วยเหลือ
3. **Sensor Adaptation**: ประโยชน์ของ Adaptive Sampling Rate คืออะไร?
  - Adaptive Sampling Rate ช่วยให้ระบบ “ปรับความถี่ในการวัด sensor ตามสถานการณ์” เพื่อประหยัดพลังงานและเพิ่มประสิทธิภาพการตอบสนอง
4. **System Health**: metrics ใดบ้างที่ควรติดตามในระบบจริง?
  - System Health metrics ที่ควรติดตามคือ Watchdog feeds/timeouts, จำนวน sensor readings, pattern changes, uptime และ free heap เพราะช่วยตรวจจับความผิดปกติของระบบ เช่น การค้าง หน่วยความจำรั่ว หรือ sensor หยุดทำงาน ก่อนที่ระบบจะล้มเหลว

# Lab 3: Advanced Timer Management & Performance 
## 📋 Advanced Analysis Questions
1. **Service Task Priority**: ผลกระทบของ Priority ต่อ Timer Accuracy?
  - Priority สูงช่วยให้ Timer Callback ถูกเรียกตรงเวลาและลด jitter แต่ถ้าสูงเกินไปอาจแย่งทรัพยากรจาก Task อื่น ควรเลือกให้เหมาะสมกับระบบ เช่น Priority = 15 สำหรับงาน Real-time
2. **Callback Performance**: วิธีการเพิ่มประสิทธิภาพ Callback Functions?
  - ลดเวลาประมวลผลใน Callback โดยไม่ทำงานหนักในนั้น ใช้ Queue หรือ Event Group ส่งข้อมูลไปยัง Task อื่นแทน เช่น ส่งสัญญาณให้ Task ประมวลผลภายหลัง
3. **Memory Management**: กลยุทธ์การจัดการ Memory สำหรับ Dynamic Timers?
  - ใช้ Static Allocation สำหรับ Timer คงที่ และ Dynamic Allocation เมื่อจำเป็น ต้องมีระบบตรวจจับ heap เหลือน้อยและลบ Timer ที่ไม่ใช้ เช่น cleanup_dynamic_timers()
4. **Error Recovery**: วิธีการ Handle Timer System Failures?
  - ใช้การ retry เมื่อ xTimerStart() หรือ xTimerStop() ล้มเหลว (เช่น safe_timer_start() ในโค้ด) พร้อมบันทึกความผิดพลาดลงใน health_data.command_failures เพื่อวิเคราะห์ภายหลัง
5. **Production Deployment**: การปรับแต่งสำหรับ Production Environment?
  - ปรับระบบให้ เสถียรและประหยัดทรัพยากร เช่น ใช้ Static Timer, เปิด Health Monitoring, ตรวจ Stack/Heap, ตั้ง Service Task Priority ให้สมดุล และปิด Debug Logs เพื่อเพิ่มประสิทธิภาพจริงในงาน Production
