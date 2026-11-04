# NAM Audio Processor — Latest Starter Project

โปรเจกต์ JUCE/C++ สำหรับ Windows 10/11, Visual Studio 2022, CMake, VST3 และ Standalone

## สถานะและขอบเขตที่ต้องทราบ

รุ่นนี้เป็น **Starter ที่ build ได้ตามโครงสร้าง JUCE** มี Input/Output gain, peak/RMS meter, bypass และเลือกไฟล์ `.nam` พร้อมบันทึก path ใน plugin state

**ยังไม่ได้เชื่อมต่อ NAM inference/DSP engine จริง** การเลือก `.nam` ในรุ่นนี้เป็นเพียงการเลือกและบันทึกตำแหน่งไฟล์ ไม่ได้ประมวลผลเสียงผ่านโมเดล NAM ดังนั้นห้ามใช้คำอธิบายนี้แทนปลั๊กอิน NAM ที่สมบูรณ์

GUI วาดด้วยโค้ด JUCE และไม่มีการอ้างอิงไฟล์ PNG ภายนอก จึงไม่ต้องมี `Assets/LA2A_Knob_Outer_Static.png` สำหรับโปรเจกต์นี้ (ไฟล์ดังกล่าวเป็น asset ของโปรเจกต์อื่น)

## Build บน Windows

ต้องติดตั้ง:
- Visual Studio 2022 พร้อม workload **Desktop development with C++**
- CMake 3.22 หรือใหม่กว่า
- Git for Windows
- อินเทอร์เน็ตสำหรับดาวน์โหลด JUCE 8.0.10 ครั้งแรกผ่าน CMake FetchContent

เปิด PowerShell ในโฟลเดอร์เดียวกับ `CMakeLists.txt` แล้วรัน:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```

ไฟล์ VST3 จะอยู่ภายใต้ `build\NAMAudioProcessor_artefacts\Release\VST3\` (ตำแหน่งย่อยอาจต่างตาม generator)

## GitHub Actions

ไฟล์ workflow: `.github/workflows/build-windows.yml`
- ตรวจเฉพาะ source files ที่โปรเจกต์นี้ต้องใช้
- ไม่บังคับ PNG asset ที่ไม่มีการใช้งาน
- Configure และ Build ด้วย Visual Studio 2022 x64
- ตรวจว่ามี bundle `.vst3` จริงก่อนอัปโหลด artifact
- หากไม่พบ VST3 จะหยุดพร้อมข้อความที่ระบุสาเหตุ

## ติดตั้ง VST3

คัดลอกทั้งโฟลเดอร์ `.vst3` ไปยัง:

`C:\Program Files\Common Files\VST3`

จากนั้นสั่ง rescan plug-ins ใน DAW

## ข้อควรระวัง

- GitHub Actions workflow ถูกเตรียมไว้ แต่ ZIP นี้ยังไม่ได้รันบน GitHub repository ของคุณ จึงยังไม่สามารถรับรองว่า CI ผ่านจริง
- การ build สำเร็จไม่ได้แปลว่า NAM DSP integration เสร็จสมบูรณ์
- ก่อนเผยแพร่ ให้ตรวจสอบ license ของ JUCE และ dependency ที่ใช้
