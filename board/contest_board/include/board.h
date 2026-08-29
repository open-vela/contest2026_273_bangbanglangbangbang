/****************************************************************************
 * boards/arm/stm32n6/stm32n647-evb/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_STM32N6_STM32N647_EVB_INCLUDE_BOARD_H
#define __BOARDS_ARM_STM32N6_STM32N647_EVB_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "hardware/stm32_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* HSE - 24MHz external oscillator */

#define STM32N6_HSE_FREQUENCY   24000000UL
#define STM32N6_HSI_FREQUENCY   64000000UL

/* PLL configuration */

#define STM32N6_PLL_FREQUENCY   600000000UL

/* AHB frequency (CPU / 3 = 200MHz) */

#define STM32N6_HCLK_FREQUENCY  200000000UL

/* UART configuration
 *
 * USART1: APB2, TX=PA9, RX=PA10
 * USART2: APB1, TX=PA2, RX=PA3
 * USART3: APB1, TX=PB10, RX=PB11
 */

#define STM32N6_USART1_BASE     0x40011000
#define STM32N6_USART1_IRQ      37

#define STM32N6_USART2_BASE     0x40004400
#define STM32N6_USART2_IRQ      38

#define STM32N6_USART3_BASE     0x40004800
#define STM32N6_USART3_IRQ      39

/* I2C pin configuration
 *
 * I2C1: PB6 (SCL, AF4), PB7 (SDA, AF4)
 * I2C2: PB10 (SCL, AF4), PB11 (SDA, AF4)
 * I2C3: PA7 (SCL, AF4), PB5 (SDA, AF4)
 *
 * All I2C pins: open-drain output, high speed, internal pull-up enabled
 */

/* LED definitions **********************************************************/

/* LED index values for use with board_autoled_on(), board_autoled_off(),
 * and board_autoled_toggle().
 */

#define LED_STARTED       0  /* LED started */
#define LED_HEAPALLOCATE  1  /* LED heap allocated */
#define LED_IRQSENABLED   2  /* LED IRQs enabled */
#define LED_STACKCREATED  3  /* LED stack created */
#define LED_INIRQ         4  /* LED in IRQ */
#define LED_SIGNAL        5  /* LED signal */
#define LED_ASSERTION     6  /* LED assertion */
#define LED_PANIC         7  /* LED panic */

/* SPI configuration ********************************************************/

/* SPI1 pin definitions (PA5=SCK, PA6=MISO, PA7=MOSI) */

#define SPI1_SCK_PORT   GPIO_PORTA
#define SPI1_SCK_PIN    5
#define SPI1_MISO_PORT  GPIO_PORTA
#define SPI1_MISO_PIN   6
#define SPI1_MOSI_PORT  GPIO_PORTA
#define SPI1_MOSI_PIN   7

/* SPI1 CS (chip select) - PA4 (directly controlled via GPIO) */

#define SPI1_CS_PORT    GPIO_PORTA
#define SPI1_CS_PIN     4

/* SPI2 pin definitions (PB13=SCK, PB14=MISO, PB15=MOSI) */

#define SPI2_SCK_PORT   GPIO_PORTB
#define SPI2_SCK_PIN    13
#define SPI2_MISO_PORT  GPIO_PORTB
#define SPI2_MISO_PIN   14
#define SPI2_MOSI_PORT  GPIO_PORTB
#define SPI2_MOSI_PIN   15

/* SPI2 CS (chip select) - PB12 (directly controlled via GPIO) */

#define SPI2_CS_PORT    GPIO_PORTB
#define SPI2_CS_PIN     12

/* SPI3 pin definitions (PB3=SCK, PB4=MISO, PB5=MOSI) */

#define SPI3_SCK_PORT   GPIO_PORTB
#define SPI3_SCK_PIN    3
#define SPI3_MISO_PORT  GPIO_PORTB
#define SPI3_MISO_PIN   4
#define SPI3_MOSI_PORT  GPIO_PORTB
#define SPI3_MOSI_PIN   5

/* SPI3 CS (chip select) - PA15 (directly controlled via GPIO) */

#define SPI3_CS_PORT    GPIO_PORTA
#define SPI3_CS_PIN     15

/* LTDC configuration - ATK-MD0700R-800480 RGBLCD panel (ID=0x7084)
 * Reference: rgblcd.c:128-151
 */

#ifdef CONFIG_STM32N6_LTDC
#  define BOARD_LTDC_WIDTH          800
#  define BOARD_LTDC_HEIGHT         480
#  define BOARD_LTDC_HSYNC          1
#  define BOARD_LTDC_VSYNC          1
#  define BOARD_LTDC_HBP            46
#  define BOARD_LTDC_VBP            23
#  define BOARD_LTDC_HFP            210
#  define BOARD_LTDC_VFP            22
#  define BOARD_LTDC_PIXEL_CLOCK    33333333  /* 33.333 MHz */
#endif

/* XSPI2 configuration for MX25UM25645G NOR Flash
 *
 * XSPI2 uses the IO Manager (XSPIM) port 2. The pins are connected
 * via the XSPIM to the external Octal SPI NOR Flash.
 *
 * Typical pin assignments for STM32N647-EVB (Port G, AF9):
 *   PG6  - XSPI2_CLK  (Clock)
 *   PG7  - XSPI2_NCS  (Chip Select)
 *   PG5  - XSPI2_DQS  (Data Strobe)
 *   PG0  - XSPI2_D0
 *   PG1  - XSPI2_D1
 *   PG2  - XSPI2_D2
 *   PG3  - XSPI2_D3
 *   PG4  - XSPI2_D4
 *   PG8  - XSPI2_D5
 *   PG9  - XSPI2_D6
 *   PG10 - XSPI2_D7
 */

#ifdef CONFIG_STM32N6_XSPI2

/* GPIO alternate function for XSPI2 via XSPIM Port 2 = AF9 */

/* XSPI2 Alternate Function (AF9 for XSPI2 on Port G) - already in hardware/stm32_gpio.h */

/* XSPI2 pin definitions (pinset encoding: port[2:0] | pin[6:3] | af[10:7]) */

#define GPIO_XSPI2_CLK       (GPIO_PORTG | (GPIO_PIN6  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_NCS       (GPIO_PORTG | (GPIO_PIN7  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_DQS       (GPIO_PORTG | (GPIO_PIN5  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D0        (GPIO_PORTG | (GPIO_PIN0  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D1        (GPIO_PORTG | (GPIO_PIN1  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D2        (GPIO_PORTG | (GPIO_PIN2  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D3        (GPIO_PORTG | (GPIO_PIN3  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D4        (GPIO_PORTG | (GPIO_PIN4  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D5        (GPIO_PORTG | (GPIO_PIN8  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D6        (GPIO_PORTG | (GPIO_PIN9  << 3) | GPIO_AF9_XSPI2)
#define GPIO_XSPI2_D7        (GPIO_PORTG | (GPIO_PIN10 << 3) | GPIO_AF9_XSPI2)

#endif /* CONFIG_STM32N6_XSPI2 */

#endif /* __BOARDS_ARM_STM32N6_STM32N647_EVB_INCLUDE_BOARD_H */
