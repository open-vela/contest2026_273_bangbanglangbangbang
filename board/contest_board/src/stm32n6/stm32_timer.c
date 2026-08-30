/****************************************************************************
 * arch/arm/src/stm32n6/stm32_timer.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>

#include "arm_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* SysTick base address */

#define SYSTICK_BASE  0xe000e010

/* SysTick register offsets */

#define SYSTICK_CSR   0x00  /* Control and Status Register */
#define SYSTICK_RVR   0x04  /* Reload Value Register */
#define SYSTICK_CVR   0x08  /* Current Value Register */
#define SYSTICK_CALIB 0x0c  /* Calibration Register */

/* SysTick CSR bits */

#define SYSTICK_CSR_ENABLE    (1 << 0)  /* Counter enable */
#define SYSTICK_CSR_TICKINT   (1 << 1)  /* interrupt enable */
#define SYSTICK_CSR_CLKSOURCE (1 << 2)  /* Clock source: 1 = processor clock */

/* Helper macros for register access */

#define SYSTICK_REG(offset) \
  (*(volatile uint32_t *)(SYSTICK_BASE + (offset)))

/* SysTick configuration for 1ms tick at 600MHz
 * Reload value = 600000000 / 1000 = 600000 = 0x927C0
 */

#define SYSTICK_RELOAD_VALUE  600000

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_timer_initialize
 *
 * Description:
 *   Initialize the SysTick timer for 1ms system tick.
 *
 ****************************************************************************/

void up_timer_initialize(void)
{
  /* Set SysTick reload value for 1ms tick */

  SYSTICK_REG(SYSTICK_RVR) = SYSTICK_RELOAD_VALUE - 1;

  /* Reset current value */

  SYSTICK_REG(SYSTICK_CVR) = 0;

  /* Enable SysTick with processor clock and interrupt */

  SYSTICK_REG(SYSTICK_CSR) = SYSTICK_CSR_ENABLE |
                              SYSTICK_CSR_TICKINT |
                              SYSTICK_CSR_CLKSOURCE;
}
