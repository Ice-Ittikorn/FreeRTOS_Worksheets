# Lab 1: Single Task vs Multitasking Demo
### Part 3: เปรียบเทียบและวิเคราะห์ (5 นาที)

## การทดสอบ

1. **ทดสอบ Single Task**:
   - สังเกตการทำงานของ LED 
       - Ledกระพริบสลับซ้ายซ้ายขวา
   - กดปุ่มและสังเกตเวลาตอบสนอง
       - ปุ่มตอบสนองช้าต้องวนมาถึง task ของสวิตถึงจะตอบสนอง
   - บันทึกเวลาตอบสนองเมื่อกดปุ่ม
       - ถ้านับจาก task จนถึง task จะต้องรอประมาณ 1.5 วิ

2. **ทดสอบ Multitasking**:
   - สังเกตการทำงานของ LED (ทำงานพร้อมกัน)
       - กระพริบพร้อมกันเเต่ดับไม่พร้อมกัน เเสดงว่า 2 task ทำงานพร้อมกัน
   - กดปุ่มและสังเกตเวลาตอบสนอง
       - ตอนสนองกันทีที่กดปุ่ม
   - เปรียบเทียบกับระบบ Single Task
       - เวลาการตอบสนองเร็วกว่า ทำงานทุกๆ task พร้อมกัน

## คำถามสำหรับวิเคราะห์

1. ความแตกต่างในการตอบสนองปุ่มระหว่างทั้งสองระบบคืออะไร?
    - ระยะเวลาในการตอบสนองที่แบบ Multitasking เร็วกว่ามาก
2. ใน Single Task System งานไหนที่ทำให้การตอบสนองล่าช้า?
    -  task 1เเละ3 เพราะมีการใช้ delay ประมาณ 2 วิ
3. ข้อดีของ Multitasking System ที่สังเกตได้คืออะไร?
    - ทำงานได้หลาย task พร้อมกัน ไม่จำเป็นต้องรอมาถึงรอตัวเอง
4. มีข้อเสียของ Multitasking System ที่สังเกตได้หรือไม่?
    - borad ทำงานอย่างเต็มที่ ใช้ทรัพยากรมาก

# Lab 2: Time-Sharing Implementation
## การทดสอบและวัดผล

### 1. การวัด CPU Utilization
- สังเกตการแสดงสถิติทุก 20 context switches
  
```
I (213441) TIME_SHARING: Context switches: 4180
I (213441) TIME_SHARING: Total time: 213156056 us
I (213441) TIME_SHARING: Task execution time: 15898597 us
I (213441) TIME_SHARING: CPU utilization: 7.5%
I (213451) TIME_SHARING: Overhead: 92.5%
I (213451) TIME_SHARING: Avg time per task: 3803 us
```

- บันทึกค่า CPU utilization และ overhead

```
CPU utilization: 7.5%
Overhead: 92.5%
```

### 2. การทดสอบ Time Slice ต่างๆ
- ทดสอบ time slice: 10ms, 25ms, 50ms, 100ms, 200ms
- เปรียบเทียบประสิทธิภาพ
Time slice สั้น → task ถูกสลับบ่อย → context switch เพิ่ม → overhead สูง
Time slice ยาว → task ทำงานต่อเนื่อง → overhead ลด → task หนักทำงานต่อเนื่องมากขึ้น

### 3. การสังเกต LED Pattern
- LED1: Sensor task (งานเบา)
- LED2: Processing task (งานหนัก)
- LED3: Actuator task (งานปานกลาง)
- LED4: Display task (งานเบา)
| LED  | Task       | Workload |  pattern |
| ---- | ---------- | -------- | ---------------- |
| LED1 | Sensor     | เบา      | กระพริบเร็ว      |
| LED2 | Processing | หนัก     | กระพริบช้า       |
| LED3 | Actuator   | ปานกลาง  | กระพริบกลาง ๆ    |
| LED4 | Display    | เบา      | กระพริบเร็ว      |

## คำถามสำหรับวิเคราะห์

1. Time slice ขนาดไหนให้ประสิทธิภาพดีที่สุด? เพราะอะไร?
  - Time slice ที่ดีที่สุด:** ~50 ms → สมดุลระหว่าง latency ของ task เบาและ throughput ของ task หนัก
2. ปัญหาอะไรที่เกิดขึ้นเมื่อ time slice สั้นเกินไป?
  - Time slice สั้นเกินไป:** Context switch บ่อย → overhead สูง → CPU ทำงานจริงน้อย
3. ปัญหาอะไรที่เกิดขึ้นเมื่อ time slice ยาวเกินไป?
  - Time slice ยาวเกินไป:** Task เบาต้องรอนาน → latency สูง → งาน real-time ช้า
4. Context switching overhead คิดเป็นกี่เปอร์เซ็นต์ของเวลาทั้งหมด?
  - Context switching overhead:** ประมาณ 92–93% ของเวลาทั้งหมด (จาก log)
5. งานไหนที่ได้รับผลกระทบมากที่สุดจากการ time-sharing?
  - งานที่ได้รับผลกระทบมากที่สุด:** Task หนัก (Processing) ถูก interrupt บ่อยเมื่อ time slice สั้น

# Lab 3: Cooperative vs Preemptive Comparison

## การทดสอบและเปรียบเทียบ

### การทดสอบ Cooperative System
1. Build และ flash โปรแกรมโดย uncomment `test_cooperative_multitasking()`
2. กดปุ่มหลายครั้งและสังเกตเวลาตอบสนอง
  - เวลาตอนสนองมีทั้ง 0 , 10 , 20 ms ตามเวลาที่กดปุ่ม
3. บันทึกเวลาตอบสนองสูงสุด
  - สูงสุดที่ 20 ms

### การทดสอบ Preemptive System
1. แก้ไขโค้ดโดย uncomment `test_preemptive_multitasking()`
2. Build และ flash ใหม่
3. กดปุ่มหลายครั้งและเปรียบเทียบเวลาตอบสนอง
  - เวลาตอนสนองที่เร็วกว่า Cooperative System
## คำถามสำหรับวิเคราะห์

1. ระบบไหนมีเวลาตอบสนองดีกว่า? เพราะอะไร?
  - Preemptive ดีกว่า เพราะสลับ task ทันที ไม่ต้องรอ task ปัจจุบันเสร็จ
2. ข้อดีของ Cooperative Multitasking คืออะไร?
  - ง่ายต่อออกแบบ
  - ประหยัด CPU/หน่วยความจำ
  - เหมาะกับงานลำดับชัดเจน
3. ข้อเสียของ Cooperative Multitasking คืออะไร?
  - ตอบสนองช้า
  - ยากคาดการณ์
  - เสี่ยงถ้า task ไม่ยอมสลับ
4. ในสถานการณ์ใดที่ Cooperative จะดีกว่า Preemptive?
  - งานไม่ต้องตอบสนองทันที
  - ลำดับชัดเจน
  - ลด overhead
5. เหตุใด Preemptive จึงเหมาะสำหรับ Real-time systems?
  -ตอบสนองทันที
  จัด priority ได้



