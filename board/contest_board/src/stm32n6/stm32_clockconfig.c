/****************************************************************************
 * arch/arm/src/stm32n6/stm32_clockconfig.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
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
 * STM32N6 Clock Configuration
 *
 * Based on STM32CubeN6 official examples:
 * - CPU Clock: 600MHz (IC1_CK)
 * - HCLK: 200MHz (AHB prescaler = 3)
 * - APB1/APB2: 200MHz
 *
 * Clock tree:
 *   HSI (64MHz) -> PLL1 (600MHz) -> IC1 (CPU) -> AHB (200MHz)
 *                                    IC2 (SYSCLK)
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>

#include "arm_internal.h"
#include "hardware/stm32_rcc.h"
#include "hardware/stm32_pwr.h"

/* System clock frequency variable */

uint32_t SystemCoreClock = 64000000;  /* Default HSI */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Flash register base */

#define FLASH_R_BASE    0x40022000
#define FLASH_ACR       (*(volatile uint32_t *)(FLASH_R_BASE + 0x00))

/* PLL1 configuration for 600MHz from 64MHz HSI
 * f_VCO = f_in * PLLN / PLLM = 64 * 75 / 4 = 1200 MHz
 * f_PLL = f_VCO / PLLP = 1200 / 2 = 600 MHz
 */

#define PLL1M_VALUE    4      /* PLL1 M divider */
#define PLL1N_VALUE    75     /* PLL1 N multiplier */
#define PLL1P_VALUE    1      /* PLL1 P divider (actual divider = value + 1) */
#define PLL1P_VALUE_ACTUAL 2  /* Actual P divider */

/* HCLK prescaler (HCLK = CPUCLK / 3 = 200MHz) */

#define AHB_PRESCALER   3

/* Timeout for clock ready */

#define CLOCK_TIMEOUT 0x10000

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int wait_for_flag(volatile uint32_t *reg, uint32_t mask, bool set)
{
  int timeout = CLOCK_TIMEOUT;

  while (timeout > 0)
    {
      uint32_t value = *reg;
      if (set && (value & mask))
        {
          return 0;
        }
      else if (!set && !(value & mask))
        {
          return 0;
        }
      timeout--;
    }

  return -1;
}

/****************************************************************************
 * Name: stm32_pwr_config
 *
 * Description:
 *   Configure power supply and voltage scaling.
 *
 ****************************************************************************/

static void stm32_pwr_config(void)
{
  volatile uint32_t *pwr_cr1 =
    (volatile uint32_t *)(STM32_PWR_BASE + PWR_CR1_OFFSET);
  volatile uint32_t *pwr_cr3 =
    (volatile uint32_t *)(STM32_PWR_BASE + PWR_CR3_OFFSET);
  volatile uint32_t *pwr_voscr =
    (volatile uint32_t *)(STM32_PWR_BASE + PWR_VOSCR_OFFSET);

  /* Enable PWR clock (on AHB4, bit 18) */

  volatile uint32_t *rcc_ahb4enr =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB4ENR_OFFSET);
  *rcc_ahb4enr |= RCC_AHB4ENR_PWREN;

  /* Configure external power supply */

  *pwr_cr3 |= PWR_CR3_BYPASS;

  /* Wait for current voltage scaling stable */

  while (!(*pwr_voscr & PWR_VOSCR_ACTVOSRDY))
    {
    }

  /* Set voltage scaling to Scale 1 (highest performance) */

  *pwr_voscr |= PWR_VOSCR_VOS;

  /* Wait for voltage scaling ready */

  while (!(*pwr_voscr & PWR_VOSCR_VOSRDY))
    {
    }
}

/****************************************************************************
 * Name: stm32_enable_vddio
 *
 * Description:
 *   Enable VDDIO2-5 power domains for GPIO ports.
 *
 ****************************************************************************/

static void stm32_enable_vddio(void)
{
  volatile uint32_t *pwr_cr3 =
    (volatile uint32_t *)(STM32_PWR_BASE + PWR_CR3_OFFSET);

  /* Enable VDDIO2 (for GPIOG, GPIOH) */

  *pwr_cr3 |= PWR_CR3_EN_VDDIO2;

  /* Enable VDDIO3 (for GPIOC, GPIOD) */

  *pwr_cr3 |= PWR_CR3_EN_VDDIO3;

  /* Enable VDDIO4 (for GPIOE, GPIOF) */

  *pwr_cr3 |= PWR_CR3_EN_VDDIO4;

  /* Enable VDDIO5 (for GPIOA, GPIOB) */

  *pwr_cr3 |= PWR_CR3_EN_VDDIO5;
}

/****************************************************************************
 * Name: stm32_enable_bus_clocks
 *
 * Description:
 *   Enable bus clocks for peripherals.
 *
 ****************************************************************************/

static void stm32_enable_bus_clocks(void)
{
  /* BUSENR: Enable all bus access (offset 0x0244) */

  volatile uint32_t *rcc_busenr =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_BUSENR_OFFSET);
  *rcc_busenr = RCC_BUSENR_ALL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_clockconfig
 *
 * Description:
 *   Configure the clocking system for 600MHz operation.
 *   HSI (64MHz) -> PLL1 (600MHz) -> IC1 (CPU) -> AHB (200MHz)
 *
 ****************************************************************************/

void stm32_clockconfig(void)
{
  uint32_t reg;

  /* Step 1: Configure power supply and voltage scaling */

  stm32_pwr_config();

  /* Step 2: Enable VDDIO power domains */

  stm32_enable_vddio();

  /* Step 3: Enable bus clocks */

  stm32_enable_bus_clocks();

  /* Step 4: Enable HSI oscillator (64MHz) */

  reg = rcc_get_cr();
  reg |= RCC_CR_HSION;
  rcc_set_cr(reg);

  /* Wait for HSI ready */

  while (!(rcc_get_cr() & RCC_CR_HSIRDY))
    {
    }

  /* Step 5: Configure PLL1
   * Source: HSI (64MHz)
   * M divider: 4
   * N multiplier: 75
   * P divider: 2 (value + 1)
   * Result: 64MHz / 4 * 75 / 2 = 600MHz
   */

  reg = RCC_PLL1CFGR_PLLSRC_HSI |
        ((PLL1M_VALUE - 1) << 4) |   /* DIVM1: bits 7:4 */
        ((PLL1N_VALUE - 1) << 8);     /* DIVN1: bits 15:8 */
  rcc_set_pll1cfgr(reg);

  reg = (PLL1P_VALUE << 0) |         /* DIVP1: bits 4:0 */
        (1 << 16);                     /* PLL1REN: Enable PLL1 R output */
  rcc_set_pll1divr(reg);

  /* Step 6: Enable PLL1 */

  reg = rcc_get_cr();
  reg |= RCC_CR_PLL1ON;
  rcc_set_cr(reg);

  /* Wait for PLL1 ready */

  while (!(rcc_get_cr() & RCC_CR_PLL1RDY))
    {
    }

  /* Step 7: Configure IC (Interconnect Clock) dividers
   * IC1 = PLL1 / 1 = 600MHz (CPU clock)
   * Note: IC1CFGR is at offset 0x00C4
   */

  volatile uint32_t *rcc_ic1cfgr =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_IC1CFGR_OFFSET);
  *rcc_ic1cfgr = (0 << 0);  /* DIV1: /1 -> 600MHz CPU */

  /* Step 8: Configure AHB prescaler (HCLK = CPUCLK / 3 = 200MHz)
   * CFGR2 is at offset 0x0024
   */

  volatile uint32_t *rcc_cfgr2 =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_CFGR2_OFFSET);
  reg = *rcc_cfgr2;
  reg &= ~(0xF << 0);       /* Clear HPRE */
  reg |= ((AHB_PRESCALER - 1) << 0);  /* HCLK = CPUCLK / 3 */
  *rcc_cfgr2 = reg;

  /* Step 9: Set flash latency for 600MHz (5 wait states) */

  FLASH_ACR = (FLASH_ACR & ~(0xF << 0)) | (5 << 0);

  /* Step 10: Switch system clock to PLL1 */

  reg = rcc_get_cfgr1();
  reg &= ~RCC_CFGR1_SW_MASK;
  reg |= RCC_CFGR1_SW_PLL1;
  rcc_set_cfgr1(reg);

  /* Wait for system clock switch complete */

  while ((rcc_get_cfgr1() & RCC_CFGR1_SWS_MASK) != RCC_CFGR1_SWS_PLL1)
    {
    }

  /* Step 11: Update SystemCoreClock variable */

  SystemCoreClock = 600000000;  /* 600 MHz */
}
