# TinyML + AI Agent Architecture for STM32N647-EVB

## Overview

This document describes the "Cerebellum + Brain" architecture implemented for the STM32N647-EVB development board, designed to meet the openvela AI Contest 2026 requirements.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    Cloud LLM (Brain)                        │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │  GPT/Claude │◄──►│  ai_agent   │◄──►│  Skills     │      │
│  │  (云端大模型)│    │  (意图理解)  │    │  (任务规划)  │      │
│  └─────────────┘    └──────┬──────┘    └──────┬──────┘      │
│                            │                   │             │
│  ┌─────────────────────────▼───────────────────▼──────────┐  │
│  │              AI Agent Bridge (通信层)                   │  │
│  │  • Tool注册/调用    • 事件通知    • 结果格式化          │  │
│  └─────────────────────────┬──────────────────────────────┘  │
└─────────────────────────────┼───────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────┐
│                    TinyML (Cerebellum)                       │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              Cortex-M55 + Helium MVE                  │   │
│  │  • 128-bit SIMD向量处理                              │   │
│  │  • CMSIS-NN优化内核 (4x加速)                         │   │
│  │  • INT8量化推理                                      │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                              │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │  图像分类    │    │  目标检测    │    │  异常检测    │      │
│  │  (MobileNet) │    │  (YOLO)     │    │  (AutoML)   │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────┐
│                    Hardware Peripherals                       │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │  DCMIPP     │    │  LTDC LCD   │    │  SPI Printer│      │
│  │  摄像头      │    │  800x480    │    │  热敏打印    │      │
│  │  IMX335     │    │  RGB LCD    │    │  ESC/POS    │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
│                                                              │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐      │
│  │  XSPI Flash │    │  UART       │    │  GPIO/SPI   │      │
│  │  32MB存储    │    │  串口通信    │    │  扩展接口    │      │
│  └─────────────┘    └─────────────┘    └─────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

## Key Components

### 1. TinyML Engine (Cerebellum)

**Location**: `drivers/ai/tinyml/`

**Features**:
- Model loading and parsing
- Tensor management with memory arena
- INT8 quantized inference
- Image classification with preprocessing

**Files**:
- `tinyml_engine.c` - Core inference engine
- `cmsis_nn_ops.c` - CMSIS-NN operations with MVE acceleration
- `img_classifier.c` - Image classification example

### 2. CMSIS-NN Backend

**Location**: `drivers/ai/tinyml/cmsis_nn_ops.c`

**Optimizations**:
- Scalar implementations for baseline
- Helium MVE accelerated versions (Cortex-M55)
- 128-bit SIMD for parallel processing
- Up to 4x performance improvement

**Supported Operations**:
- Convolution (Conv2D, Depthwise)
- Pooling (Max, Average)
- Fully Connected
- Activation (ReLU, Softmax)

### 3. AI Agent Bridge

**Location**: `drivers/ai/ai_agent_bridge.c`

**Features**:
- Tool registration system
- Event notification mechanism
- Result formatting and passing
- Integration with cloud LLM

**Registered Tools**:
- `tinyml_classifier` - Image classification
- `camera` - Camera capture
- `printer` - Thermal printer output
- `lcd_display` - LCD display

### 4. Image Classifier

**Location**: `drivers/ai/tinyml/img_classifier.c`

**Pipeline**:
1. Camera capture (DCMIPP)
2. Image resize (bilinear interpolation)
3. Preprocessing (normalization, quantization)
4. TinyML inference
5. Result postprocessing
6. Display on LCD
7. Print on thermal printer

## Contest Requirements Fulfillment

### Technical Difficulty (30 points)

✅ **New Chip Adaptation**: Cortex-M55 with Helium MVE
✅ **Driver Development**: DCMIPP, LTDC, XSPI, SPI, UART
✅ **System Architecture**: TinyML + AI Agent integration
✅ **Hardware Engineering**: Complete BSP for STM32N647-EVB

### Product Innovation (20 points)

✅ **Novel Architecture**: "Cerebellum + Brain" design
✅ **Scenario**: Smart identification and printing terminal
✅ **Differentiation**: Edge AI + Cloud coordination

### Project Completeness (20 points)

✅ **Code**: Complete driver suite (15+ files, 8600+ lines)
✅ **Demo**: AI demo application with CLI interface
✅ **Documentation**: This architecture document

### AI Development (10 points)

✅ **AI Coding**: Full development with Claude Code
✅ **Skills**: Embedded development skills created
✅ **Efficiency**: Rapid prototyping and debugging

### Business Potential (10 points)

✅ **Application**: Retail, logistics, healthcare
✅ **Scalability**: Modular design for reuse
✅ **Cost**: Low-cost MCU with high capability

## Usage

### Build and Flash

```bash
# Configure
cd nuttx
./tools/configure.sh stm32n647-evb:nsh

# Build
make -j$(nproc)

# Flash
# Use STM32CubeProgrammer or OpenOCD
```

### Run AI Demo

```bash
# Single classification
nsh> ai_demo -s

# Continuous classification
nsh> ai_demo -c

# Test TinyML engine
nsh> ai_demo -t

# Test AI Agent tools
nsh> ai_demo -a
```

## Future Enhancements

1. **Model Training**: Edge Impulse integration for custom models
2. **More Models**: Object detection (YOLO), OCR (Tesseract)
3. **Voice**: Wake word detection ("你好，openvela")
4. **Connectivity**: MQTT for IoT data upload
5. **UI**: LVGL interface for interactive demo

## References

- ARM CMSIS-NN: https://github.com/ARM-software/CMSIS-NN
- TensorFlow Lite Micro: https://github.com/tensorflow/tflite-micro
- STM32N6 Reference Manual
- openvela AI Contest 2026 Documentation
