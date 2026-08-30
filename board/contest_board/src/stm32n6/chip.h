/****************************************************************************
 * arch/arm/src/stm32n6/chip.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_CHIP_H
#define __ARCH_ARM_SRC_STM32N6_CHIP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* STM32N6 Memory Map */

#define STM32N6_FLASH_BASE    0x08000000
#define STM32N6_SRAM_BASE     0x20000000

/* STM32N6 Peripheral Base Addresses */

#define STM32N6_RCC_BASE      0x44020C00
#define STM32N6_GPIOA_BASE    0x42020000
#define STM32N6_GPIOB_BASE    0x42020400
#define STM32N6_GPIOC_BASE    0x42020800
#define STM32N6_GPIOD_BASE    0x42020C00
#define STM32N6_GPIOE_BASE    0x42021000
#define STM32N6_GPIOF_BASE    0x42021400
#define STM32N6_GPIOG_BASE    0x42021800
#define STM32N6_GPIOH_BASE    0x42021C00
#define STM32N6_USART1_BASE   0x40011000
#define STM32N6_USART2_BASE   0x40004400
#define STM32N6_USART3_BASE   0x40004800

/* I2C Peripheral Base Addresses (I2C1/2/3 on APB1)
 * Note: STM32N647 has I2C1-3 and I3C1-2. I3C1 at 0x40006000 has I2C
 * backward compatibility. CONFIG_STM32N6_I2C4 maps to I3C1 hardware.
 */

#define STM32N6_I2C1_BASE     0x40005400
#define STM32N6_I2C2_BASE     0x40005800
#define STM32N6_I2C3_BASE     0x40005C00
#define STM32N6_I2C4_BASE     0x40006000  /* Actually I3C1, I2C-compatible */

/* SPI Peripheral Base Addresses */

#define STM32N6_SPI1_BASE     0x40013000
#define STM32N6_SPI2_BASE     0x40003800
#define STM32N6_SPI3_BASE     0x40003C00

/* XSPI Peripheral Base Addresses (non-secure) */

#define STM32N6_XSPI1_BASE    0x420C5000  /* XSPI1 register base */
#define STM32N6_XSPI2_BASE    0x420CA000  /* XSPI2 register base */
#define STM32N6_XSPI3_BASE    0x420CD000  /* XSPI3 register base */
#define STM32N6_XSPIM_BASE    0x420CB400  /* XSPI IO Manager base */

/* XSPI Memory-Mapped Base Addresses (AXI) */

#define STM32N6_XSPI1_MM_BASE 0x90000000  /* XSPI1 memory-mapped */
#define STM32N6_XSPI2_MM_BASE 0x70000000  /* XSPI2 memory-mapped */
#define STM32N6_XSPI3_MM_BASE 0x80000000  /* XSPI3 memory-mapped */

/* System Clock Frequencies
 * CPU: 600MHz (PLL1 output)
 * HCLK: 200MHz (CPU / 3)
 * APB1/APB2: 200MHz
 */

#define STM32N6_CPUCLK_FREQUENCY  600000000UL  /* 600 MHz CPU */
#define STM32N6_HCLK_FREQUENCY    200000000UL  /* 200 MHz HCLK */
#define STM32N6_PCLK1_FREQUENCY   200000000UL  /* 200 MHz APB1 */
#define STM32N6_PCLK2_FREQUENCY   200000000UL  /* 200 MHz APB2 */

/* IRQ Numbers (from CMSIS stm32n647xx.h) */

#define STM32_IRQ_DCMIPP        48   /* DCMIPP global interrupt */

/* Number of peripheral interrupts */

#define STM32_IRQ_NEXTINTS      128

#endif /* __ARCH_ARM_SRC_STM32N6_CHIP_H */
