# STM32N647-EVB Board - openvela Port

## 概述

本项目将 openvela (NuttX) 移植到 **STM32N647-EVB** 开发板（ARM Cortex-M55），属于 **新硬件平台适配** 赛道。

## 硬件信息

| 项目 | 规格 |
|------|------|
| MCU | STM32N647 (Cortex-M55, ARMv8-M) |
| 主频 | 600 MHz (PLL) |
| Flash | 2 MB |
| RAM | 512 KB |
| 外部晶振 | 24 MHz HSE |

## 已支持功能

- **UART** - USART1 (115200 8N1), 串口控制台
- **GPIO** - LED 控制, 通用 GPIO
- **SPI** - SPI1/SPI2/SPI3 全部支持
- **I2C** - I2C1/I2C2/I2C3 (需在 menuconfig 中启用)
- **LTDC** - LCD 显示控制器 (800x480)
- **时钟** - HSE/HSI/PLL 完整时钟树配置
- **NVIC** - 中断控制器

## 编译方法

```bash
# 进入 openvela 工作区根目录
cd ..

# 编译 (board config 路径相对于 vendor/openvela/boards/)
./build.sh contest2026_273_board/configs/nsh
```

## 烧录与运行

编译完成后，使用 ST-Link 或 J-Link 烧录 `nuttx.bin` 到开发板。

串口连接 USART1 (115200 8N1)，上电后进入 NSH 控制台。

## 目录结构

```
board/contest_board/
├── CMakeLists.txt          # 顶层 CMake
├── Kconfig                 # 板级 Kconfig
├── README.md               # 本文件
├── configs/
│   └── nsh/
│       ├── defconfig       # NSH 默认配置
│       └── Make.defs       # Make 定义
├── include/
│   └── board.h             # 板级硬件定义 (时钟/SPI/LTDC 引脚)
├── scripts/
│   ├── ld.script           # 链接脚本
│   └── Make.defs           # Make 定义
└── src/
    ├── CMakeLists.txt      # 源文件 CMake
    ├── board_init.c        # 板级初始化
    ├── board_appinit.c     # 应用初始化
    └── stm32_spi.c         # SPI 驱动
```

## 队伍信息

- 队伍编号: 273
- 队名: bangbanglangbangbang
- 赛道: 新硬件平台适配
