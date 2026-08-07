# 番茄时钟 (Pomodoro Timer) — ESP32-S3

基于 **M5Stack Cardputer** 的番茄工作法计时器。硬件层使用 `M5Unified` /
`M5Cardputer` 官方库（屏幕、键盘、扬声器、BMI270 均由其驱动，**不覆盖任何底层驱动**），
业务逻辑在 `src/pomodoro.{h,cpp}` 中与硬件完全解耦。

## 功能

- 标准番茄工作法：学习 25min / 短休息 5min / 每 4 个番茄自动进入长休息 20min
- 按键交互：空格=启动/暂停，`S`=跳过当前阶段，`R`=重置
- 白底黑字居中界面：阶段名称 + 剩余时分秒（大字号）+ 已完成番茄数 + 周期进度圆点
- 倒计时归零：扬声器提示音持续 2 秒，随后自动进入下一阶段
- 陀螺仪（BMI270）扩展：连续大幅晃动 3 秒自动暂停计时

## 工程结构

```
pomodoro-esp32/
├── platformio.ini        # 构建配置
├── include/config.h      # 时长/提示音/晃动灵敏度等全部可调参数
└── src/
    ├── main.cpp          # 硬件接入：屏幕、键盘、扬声器、IMU
    ├── pomodoro.h        # 番茄状态机接口（纯逻辑）
    └── pomodoro.cpp      # 状态机实现
```

## 编译 / 烧录

```bash
cd pomodoro-esp32
pio run                     # 编译
pio run -t upload           # 烧录（会自动识别 /dev/cu.usbmodem1101）
pio device monitor -b 115200 # 查看串口日志
```

## 硬件适配

代码默认针对 M5Stack Cardputer（内置 ST7789 屏幕 / QWERTY 键盘 / I2S 扬声器 /
BMI270，ESP32-S3 原生 USB）。若你的板子不同：

1. `platformio.ini` 中修改 `board =`（如 `esp32-s3-devkitc-1`）。
2. 键盘、屏幕、扬声器改用你现有驱动库，在 `main.cpp` 中替换对应调用即可——
   状态机逻辑无需改动。

## 参数调节

所有可调参数集中在 `include/config.h`：
- `WORK_MS` / `SHORT_BREAK_MS` / `LONG_BREAK_MS`：各阶段时长
- `BEEP_FREQ_HZ` / `BEEP_MS` / `BEEP_VOLUME`：提示音
- `SHAKE_GYRO_THRESHOLD` / `SHAKE_TRIGGER_MS`：晃动灵敏度
