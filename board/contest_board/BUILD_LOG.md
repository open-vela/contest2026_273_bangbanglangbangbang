# 编译日志 - Contest 2026 Team #273

> 日期: 2026-08-31
> 平台: STM32N647-EVB (Cortex-M55, ARMv8-M)
> 工具链: arm-none-eabi-gcc 10.3.1

## 1. 编译结果

| 项目 | 状态 |
|------|------|
| 目标板 | contest2026_273_board:nsh |
| 固件大小 | nuttx.bin 274K / nuttx.hex 768K |
| 编译结果 | **成功** |

### 包含的程序

| 程序 | 入口地址 | 说明 |
|------|----------|------|
| `hello` | 0x0803c178 | Hello World 示例 |
| `gpio` | 0x0803ba60 | GPIO 控制示例 |
| `camera` | 0x0803b468 | DCMIPP 摄像头采集 |

### 启用的硬件驱动

- USART1 (115200 8N1 串口控制台)
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

**处理:** 仅为警告，不影响编译。

### 错误 6: DCMIPP board 级隐式函数声明

**现象:**
```
stm32_dcmipp.c:138:3: warning: implicit declaration of function 'stm32_dcmipp_imgdata_register'
```

**原因:** board 级 DCMIPP 初始化调用了 `stm32_dcmipp_imgdata_register`，但该函数的头文件声明未被包含。

**处理:** 仅为警告，链接时能找到符号。

---

## 3. 已知待解决问题

| 问题 | 优先级 | 说明 |
|------|--------|------|
| LTDC 驱动重写 | 高 | 需要适配正确的 RCC 寄存器和 GPIO API |
| AI 框架集成 | 中 | drivers/ai/ 目录的 Kconfig 未接入构建系统 |
| DCMIPP imgdata 警告 | 低 | 修正格式字符串 |
| fb 帧缓冲示例 | 中 | 依赖 LTDC 完成后才能启用 |

---

## 4. 编译命令

```bash
cd /home/vant/nuttx
make distclean
./tools/configure.sh contest2026_273_board:nsh
make -j2
```

## 5. 烧录文件

固件已拷贝到共享目录:
- `\\vmware-host\Shared Folders\n647\nuttx_contest_273.bin` (274K)
- `\\vmware-host\Shared Folders\n647\nuttx_contest_273.hex` (768K)

使用 STM32CubeProgrammer 烧录到地址 `0x08000000`。
