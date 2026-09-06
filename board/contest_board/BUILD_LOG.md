# 编译日志 - Contest 2026 Team #273

> 日期: 2026-09-06 (更新)
> 平台: STM32N647-EVB (Cortex-M55, ARMv8-M)
> 工具链: arm-none-eabi-gcc 10.3.1

## 1. 编译结果

| 项目 | 状态 |
|------|------|
| 目标板 | contest2026_273_board:nsh |
| 固件大小 | nuttx.bin 284K / nuttx.hex 801K |
| 编译结果 | **成功 (0 warnings, 0 errors)** |

### 包含的程序

| 程序 | 入口地址 | 说明 |
|------|----------|------|
| `hello` | 0x0803c178 | Hello World 示例 |
| `gpio` | 0x0803ba60 | GPIO 控制示例 |
| `camera` | 0x0803b468 | DCMIPP 摄像头采集 |

### 启用的硬件驱动

- USART1 (115200 8N1 串口控制台, PA9/PA10)
- USART2 (115200 8N1 调试日志, PA2/PA3) — 2026-09-06 新增
- SPI1 (热敏打印机接口)
- I2C1 (摄像头 SCCB 控制)
- DCMIPP (摄像头管道，支持 IMX335/OV5640)
- XSPI2 (外部 NOR Flash, MX25UM25645G)
- GPIO (LED/按键)

---

## 2. 遇到的错误与修复

### 错误 1: LTDC 驱动 fb_vtable_s API 不匹配

**现象:**
```
chip/stm32_ltdc.c:561:15: error: 'struct fb_vtable_s' has no member named 'read'
chip/stm32_ltdc.c:562:15: error: 'struct fb_vtable_s' has no member named 'write'
chip/stm32_ltdc.c:563:15: error: 'struct fb_vtable_s' has no member named 'setcursor'
chip/stm32_ltdc.c:564:15: error: 'struct fb_vtable_s' has no member named 'setcursorshape'
```

**原因:** LTDC 驱动中使用了 `fb_vtable_s` 结构体中不存在的成员 (`read`, `write`, `setcursorshape`)。上游 NuttX 的 `fb_vtable_s` 仅包含 `getvideoinfo`, `getplaneinfo`, `open`, `close`，以及条件编译的 `getcmap`/`putcmap` 和 `getcursor`/`setcursor`。

**修复:** 删除了 `ltdc_read`, `ltdc_write`, `ltdc_setcursorshape` 函数及其 vtable 赋值，将 `setcursor` 用 `#ifdef CONFIG_FB_HWCURSOR` 保护。

**状态:** 部分修复（函数签名和 vtable 赋值已修正）

### 错误 2: LTDC 驱动 stm32_gpio_write 参数错误

**现象:**
```
chip/stm32_ltdc.c:598:3: error: too few arguments to function 'stm32_gpio_write'
```

**原因:** `stm32_gpio_write` 需要 3 个参数 `(port, pin, value)`，代码中只传了 2 个 `(GPIO_PORTA | GPIO_PIN3, true)`。

**修复:** 改为 `stm32_gpio_write(GPIO_PORTA, GPIO_PIN3, true)`

### 错误 3: LTDC 驱动 RCC 寄存器未定义

**现象:**
```
chip/stm32_ltdc.c:228:16: error: 'RCC_IC16CFGR_IC16SRC_PLL1' undeclared
chip/stm32_ltdc.c:232:51: error: 'RCC_CCIPR5_LTDCSEL_MASK' undeclared
```

**原因:** LTDC 时钟配置使用了 STM32N6 RCC 中未定义的寄存器位域宏。这些宏在当前的硬件头文件中不存在。

**处理:** 由于 LTDC 驱动存在大量未定义寄存器和 GPIO 配置函数签名不匹配等问题，暂时**禁用 LTDC**，先保证其他功能可用。LTDC 驱动需要后续完整重写。

### 错误 4: LTDC 驱动 stm32_gpio_config 参数不匹配

**现象:**
```
chip/stm32_ltdc.c:293:3: error: too few arguments to function 'stm32_gpio_config'
chip/stm32_ltdc.c:293:59: error: 'GPIO_PUSHPULL' undeclared
chip/stm32_ltdc.c:293:75: error: 'GPIO_SPEED_HIGH' undeclared
```

**原因:** LTDC 引脚配置使用的 GPIO 常量和函数签名与当前 STM32N6 GPIO 驱动不匹配。

### 错误 5: DCMIPP imgdata 格式字符串警告

**现象:**
```
chip/stm32_dcmipp_imgdata.c:249:14: warning: format '%d' expects argument of type 'int', but argument 3 has type 'uint32_t'
```

**原因:** `%d` 格式说明符用于 `uint32_t` 类型，应使用 `%lu` 或 `PRIu32`。

**修复:** 改用 `%lu` 并强制转换为 `(unsigned long)`。

**状态:** ✅ 已修复 (2026-09-01)

### 错误 6: DCMIPP board 级隐式函数声明

**现象:**
```
stm32_dcmipp.c:138:3: warning: implicit declaration of function 'stm32_dcmipp_imgdata_register'
```

**原因:** board 级 DCMIPP 初始化调用了 `stm32_dcmipp_imgdata_register`，但该函数的头文件声明未被包含。

**修复:** 在 `stm32_dcmipp.h` 中添加函数声明。

**状态:** ✅ 已修复 (2026-09-01)

### 错误 7: 未使用的函数和变量警告

**现象:**
```
chip/stm32_clockconfig.c:80:12: warning: 'wait_for_flag' defined but not used
chip/stm32_xspi.c:367:12: warning: unused variable 'regval'
chip/stm32_xspi.c:810:12: warning: 'xspi_wait_write_complete' defined but not used
chip/stm32_clockconfig.c:90:22: warning: unused variable 'pwr_cr1'
```

**原因:** 代码中定义了未使用的函数和变量。

**修复:**
- 删除 `stm32_clockconfig.c` 中未使用的 `wait_for_flag` 函数和 `pwr_cr1` 变量
- 删除 `stm32_xspi.c` 中未使用的 `regval` 变量和 `xspi_wait_write_complete` 函数

**状态:** ✅ 已修复 (2026-09-01)

### 错误 8: IC16 分频器寄存器定义错误 (2026-09-06)

**现象:**
LTDC 像素时钟配置错误，实际频率 200MHz 而非预期 25MHz。

**原因:**
RCC 头文件中 IC16CFGR 寄存器位域定义错误：
- 旧定义: `IC16DIV_SHIFT=4`, `IC16DIV_MASK=0x7` (3位，最大分频8)
- 实际: `IC16DIV_SHIFT=16`, `IC16DIV_MASK=0xFF` (8位，最大分频256)

代码写入 `(64-1)=63` 被截断为 `63 & 0x7 = 7`，实际分频器为 8。

**修复:**
参考 STM32N6xx CMSIS 头文件 (stm32n657xx.h)，修正 IC16CFGR 寄存器位域定义：
- `IC16SEL`: bits [29:28] - 源选择
- `IC16INT`: bits [23:16] - 整数分频因子 (8位)

**状态:** ✅ 已修复 (2026-09-06)

### 错误 9: ISR 中调用 gettimeofday (2026-09-06)

**现象:**
DCMIPP 帧完成回调在 ISR 上下文中调用 `gettimeofday()`，可能导致死锁。

**原因:**
`stm32_dcmipp_irq_handler` (ISR) → `g_pipe_callback[pipe]` → `dcmipp_frame_done` → `gettimeofday()`

`gettimeofday()` 是 libc 函数，内部可能获取锁或访问非中断安全的数据结构。

**修复:**
使用 NuttX 工作队列延迟处理帧完成回调：
1. ISR 中仅调用 `work_queue(HPWORK, ...)` 调度延迟工作
2. 工作队列处理函数中调用 `gettimeofday()` 和回调

**状态:** ✅ 已修复 (2026-09-06)

### 错误 10: LTDC GPIO 速度设置过低 (2026-09-06)

**现象:**
LTDC 显示花屏或无显示。

**原因:**
GPIO 速度从 `GPIO_OSPEEDR_HIGH` (50-100MHz) 改为 `GPIO_OSPEEDR_LOW` (~2-8MHz)。25MHz 像素时钟需要至少 `GPIO_OSPEEDR_MED` (25-50MHz)。

**修复:**
将所有 LTDC 数据/时钟引脚恢复为 `GPIO_OSPEEDR_HIGH`。

**状态:** ✅ 已修复 (2026-09-06)

---

## 3. 已知待解决问题

| 问题 | 优先级 | 说明 |
|------|--------|------|
| LTDC 驱动测试 | 高 | 需要在开发板上测试 LCD 显示 |
| AI 框架集成 | 中 | drivers/ai/ 目录的 Kconfig 未接入构建系统 |
| fb 帧缓冲示例 | 中 | 依赖 LTDC 完成后才能启用 |

---

## 4. 双串口支持 (2026-09-06)

### 改动内容

新增 USART2 作为独立调试日志串口，将 printf/syslog 与 NSH 控制台分离：

**1. 新增 USART2 驱动支持:**
- Kconfig: 新增 `STM32N6_USART2` 及相关配置项
- GPIO: PA2 (TX, AF7), PA3 (RX, AF7) 初始化
- 串口注册: `/dev/ttyS1` (UART2)

**2. syslog 分离:**
- ❌ 旧: `CONFIG_SYSLOG_CHAR_CONSOLE=y` (syslog 走控制台)
- ✅ 新: `CONFIG_SYSLOG_CHAR=y` + `CONFIG_SYSLOG_DEVPATH="/dev/ttyS1"` (syslog 走 USART2)

**3. 串口分配:**

| 串口 | 引脚 | 用途 | 设备节点 |
|------|------|------|----------|
| USART1 | PA9/PA10 | NSH 控制台 | `/dev/console` |
| USART2 | PA2/PA3 | printf/syslog 调试日志 | `/dev/ttyS1` |

**修改的文件:**
- `src/stm32n6/hardware/stm32_gpio.h` — 新增 `GPIO_AF7_USART2/USART3`
- `src/stm32n6/stm32_gpio.c` — 新增 USART2 GPIO 初始化
- `src/stm32n6/Kconfig` — 新增 USART2 配置项
- `configs/nsh/defconfig` — 启用 USART2, 修改 syslog 配置

---

## 5. LTDC 驱动修复 (2026-09-03)

### 修复内容

基于 STM32N6570-DK 官方示例修复 LTDC 驱动:

**1. 时钟配置修复:**
- ❌ 旧: PLL1 -> IC16 (divider 18) -> LTDC (33MHz)
- ✅ 新: PLL4 -> IC16 (divider 64) -> LTDC (25MHz)

**2. GPIO 引脚映射修复:**
- ❌ 旧: 使用了不存在的引脚映射 (PA5=CLK, PA9=B5, PA10=B4, PA11=B3, PG0=VSYNC, PG9=R7, PB10=G7)
- ✅ 新: 严格按照官方示例 (PB13=CLK, PH6=B5, PH3=B4, PG6=B3, PE11=VSYNC, PD8=R7, PG8=G7)

**3. PLL4 配置:**
- 在 `stm32_clockconfig.c` 中添加 PLL4 初始化
- PLL4 源: HSI (64MHz)
- PLL4 倍频: * 25 = 1600MHz
- IC16 分频: / 64 = 25MHz (像素时钟)

**参考文件:**
- `/home/vant/STM32CubeN6/Projects/STM32N6570-DK/Examples/LTDC/LTDC_Horizontal_Mirroring/FSBL/Src/stm32n6xx_hal_msp.c`
- `/home/vant/STM32CubeN6/Projects/STM32N6570-DK/Examples/LTDC/LTDC_Horizontal_Mirroring/FSBL/Src/main.c`

---

## 7. 编译命令

```bash
cd /home/vant/nuttx
make distclean
./tools/configure.sh contest2026_273_board:nsh
make -j$(nproc)
```

## 8. 烧录方式

### 方式 1: st-flash (SWD, 需安装 stlink-tools)
```bash
sudo apt install stlink-tools
st-flash write nuttx.bin 0x08000000
```

### 方式 2: STM32CubeProgrammer (Windows)
```cmd
STM32_Programmer_CLI -c port=SWD freq=4000 -w nuttx.bin 0x08000000 -v -rst
```

### 方式 3: 串口 bootloader (需进入 BOOT0 模式)
```bash
sudo apt install stm32flash
# 设置 BOOT0=HIGH, 复位进入 bootloader
stm32flash -w nuttx.bin -v -g 0x08000000 /dev/ttyUSB0
```

## 9. 固件输出

- `nuttx.bin` (280K)
- `nuttx.hex` (785K)

烧录到 Flash 地址 `0x08000000`。
