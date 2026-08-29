/****************************************************************************
 * arch/arm/src/stm32n6/stm32_lowputc.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <arch/board/board.h>

#include "arm_internal.h"
#include "hardware/stm32_usart.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* USART1 base address */

#define USART1_BASE  0x40011000

/* Helper macros for register access */

#define USART1_REG(offset) \
  (*(volatile uint32_t *)(USART1_BASE + (offset)))

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_usart1_init
 *
 * Description:
 *   Initialize USART1 for console output.
 *   Uses HSI = 64MHz (before PLL), 115200 baud.
 *
 ****************************************************************************/

static void stm32_usart1_init(void)
{
  /* Enable USART1 by setting UE bit in CR1 */

  USART1_REG(STM32_USART_CR1_OFFSET) = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

  /* Set baud rate: BRR = fck / baudrate
   * Early boot uses HSI = 64MHz (before PLL configuration)
   * For 64MHz clock and 115200 baud: BRR = 64000000 / 115200 = 555 = 0x22B
   * After PLL: PCLK2 = 200MHz, BRR = 200000000 / 115200 = 1736 = 0x6C8
   */

  USART1_REG(STM32_USART_BRR_OFFSET) = 0x22B;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Output a byte to the console UART.
 *
 ****************************************************************************/

void up_putc(int ch)
{
  /* Wait for TX buffer empty (TXE flag in ISR) */

  while (!(USART1_REG(STM32_USART_ISR_OFFSET) & USART_ISR_TXE))
    {
    }

  /* Write character to TDR */

  USART1_REG(STM32_USART_TDR_OFFSET) = (uint32_t)ch;

  /* Wait for transmission complete */

  while (!(USART1_REG(STM32_USART_ISR_OFFSET) & USART_ISR_TC))
    {
    }
}

/****************************************************************************
 * Name: arm_lowputc
 *
 * Description:
 *   Output a byte to the console UART (low-level).
 *
 ****************************************************************************/

void arm_lowputc(char ch)
{
  /* Initialize USART1 on first call */

  static bool initialized = false;
  if (!initialized)
    {
      stm32_usart1_init();
      initialized = true;
    }

  up_putc(ch);
}
