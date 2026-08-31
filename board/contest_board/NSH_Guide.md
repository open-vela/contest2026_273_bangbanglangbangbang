# NSH 快速上手指南

> STM32N647-EVB | Contest 2026 Team #273

## 1. 连接串口

1. 用 USB 线连接开发板的 **USART1** 接口（或通过 ST-Link 虚拟串口）
2. 打开串口工具（MobaXterm / PuTTY / minicom）
3. 设置：**115200 波特率，8 数据位，无校验，1 停止位 (8N1)**
4. 按开发板 **RESET** 键，看到 `nsh>` 提示符即成功

## 2. 基本命令

### 文件系统
```
nsh> ls /              # 列出根目录
nsh> cat /proc/version # 查看 NuttX 版本
nsh> cat /proc/uptime  # 查看运行时间
nsh> df                # 查看磁盘使用
nsh> pwd               # 当前目录
nsh> mkdir /tmp/test   # 创建目录
```

### 系统信息
```
nsh> uname -a          # 系统信息
nsh> ps                # 查看进程列表
nsh> free              # 内存使用情况
nsh> cat /proc/cpuinfo # CPU 信息
```

### 运行程序
```
nsh> hello             # 运行 hello 示例
nsh> fb                # 运行 LCD 帧缓冲测试
nsh> camera            # 运行摄像头采集
nsh> help              # 查看所有可用命令
```

### 环境变量
```
nsh> export MYVAR=hello  # 设置环境变量
nsh> echo $MYVAR         # 读取环境变量
nsh> unset MYVAR         # 删除环境变量
```

## 3. 本固件包含的程序

| 程序名 | 说明 | 运行命令 |
|--------|------|----------|
| `hello` | 最简示例，打印 Hello World | `hello` |
| `gpio` | GPIO 控制示例，读写 LED/按键 | `gpio` |
| `camera` | 摄像头采集示例，从 DCMIPP 捕获图像 | `camera` |

## 4. 常见问题

### 无输出
- 检查串口线是否连到 **USART1**（不是 USART2/3）
- 确认波特率 115200
- 按几次回车唤醒终端

### 命令找不到
- 输入 `help` 查看所有可用命令
- 确认程序名拼写正确（区分大小写）

### 程序崩溃
- 查看串口输出的错误信息
- 用 `ps` 查看进程状态
- 用 `free` 检查内存是否充足

## 5. 编译方法

```bash
cd /home/vant/nuttx
make distclean
./tools/configure.sh contest2026_273_board:nsh
make -j2
```

编译产物：
- `nuttx.bin` — 烧录用二进制文件
- `nuttx.hex` — 烧录用 HEX 文件

## 6. 烧录方法

### 方式 A：STM32CubeProgrammer（Windows）
1. 连接 ST-Link USB
2. 打开 STM32CubeProgrammer
3. 选择 ST-Link → Connect
4. 加载 `nuttx.hex`，地址 `0x08000000`
5. 点 Download

### 方式 B：命令行（Linux，需安装 stlink-tools）
```bash
sudo apt install stlink-tools
st-flash write nuttx.bin 0x08000000
```
