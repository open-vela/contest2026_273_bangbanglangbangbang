/****************************************************************************
 * arch/arm/src/stm32n6/stm32_irq.c
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include "arm_internal.h"
#include "hardware/stm32_nvic.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Helper macros for register access */

#define NVIC_REG(offset) \
  (*(volatile uint32_t *)(NVIC_BASE + (offset)))

#define SCB_REG(offset) \
  (*(volatile uint32_t *)(SCB_BASE + (offset)))

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_irqinitialize
 *
 * Description:
 *   Initialize the interrupt subsystem.
 *
 ****************************************************************************/

void up_irqinitialize(void)
{
  /* Disable all interrupts - 4 banks for 128 IRQs (IRQ 0-127) */

  NVIC_REG(NVIC_ICER0_OFFSET) = 0xffffffff;  /* ICER0: IRQ 0-31 */
  NVIC_REG(NVIC_ICER1_OFFSET) = 0xffffffff;  /* ICER1: IRQ 32-63 */
  NVIC_REG(NVIC_ICER2_OFFSET) = 0xffffffff;  /* ICER2: IRQ 64-95 */
  NVIC_REG(NVIC_ICER3_OFFSET) = 0xffffffff;  /* ICER3: IRQ 96-127 */

  /* Clear all pending interrupts - 4 banks */

  NVIC_REG(NVIC_ICPR0_OFFSET) = 0xffffffff;  /* ICPR0: IRQ 0-31 */
  NVIC_REG(NVIC_ICPR1_OFFSET) = 0xffffffff;  /* ICPR1: IRQ 32-63 */
  NVIC_REG(NVIC_ICPR2_OFFSET) = 0xffffffff;  /* ICPR2: IRQ 64-95 */
  NVIC_REG(NVIC_ICPR3_OFFSET) = 0xffffffff;  /* ICPR3: IRQ 96-127 */

  /* Set VTOR to the vector table address */

  SCB_REG(SCB_VTOR_OFFSET) = (uint32_t)&_vectors;

  /* Set AIRCR: 4 bits preemption priority, 0 bits sub-priority
   * VECTKEY (0x05FA) must be written to bits [31:16] for the write
   * to take effect. PRIGROUP = 3 (bits [10:8]) gives 16 preemption
   * levels and 0 sub-priority levels with 4 priority bits.
   */

  SCB_REG(SCB_AIRCR_OFFSET) = 0x05fa0300;

  /* Set PendSV and SysTick to lowest priority.
   * SHPR3 (SCB offset 0x20): byte[3] = SysTick (exception 15),
   * byte[2] = PendSV (exception 14). Write 0xff to both.
   */

  SCB_REG(SCB_SHPR3_OFFSET) = 0xff000000;

  /* Enable interrupts */

  up_irq_enable();
}

/****************************************************************************
 * Name: up_disable_irq
 *
 * Description:
 *   Disable the specified IRQ.
 *
 ****************************************************************************/

void up_disable_irq(int irq)
{
  if (irq < 32)
    {
      NVIC_REG(NVIC_ICER0_OFFSET) = (1 << irq);
    }
  else if (irq < 64)
    {
      NVIC_REG(NVIC_ICER1_OFFSET) = (1 << (irq - 32));
    }
  else if (irq < 96)
    {
      NVIC_REG(NVIC_ICER2_OFFSET) = (1 << (irq - 64));
    }
  else if (irq < 128)
    {
      NVIC_REG(NVIC_ICER3_OFFSET) = (1 << (irq - 96));
    }
}

/****************************************************************************
 * Name: up_enable_irq
 *
 * Description:
 *   Enable the specified IRQ.
 *
 ****************************************************************************/

void up_enable_irq(int irq)
{
  if (irq < 32)
    {
      NVIC_REG(NVIC_ISER0_OFFSET) = (1 << irq);
    }
  else if (irq < 64)
    {
      NVIC_REG(NVIC_ISER1_OFFSET) = (1 << (irq - 32));
    }
  else if (irq < 96)
    {
      NVIC_REG(NVIC_ISER2_OFFSET) = (1 << (irq - 64));
    }
  else if (irq < 128)
    {
      NVIC_REG(NVIC_ISER3_OFFSET) = (1 << (irq - 96));
    }
}

/****************************************************************************
 * Name: arm_ack_irq
 *
 * Description:
 *   Acknowledge the IRQ.
 *
 ****************************************************************************/

void arm_ack_irq(int irq)
{
  if (irq < 32)
    {
      NVIC_REG(NVIC_ICPR0_OFFSET) = (1 << irq);
    }
  else if (irq < 64)
    {
      NVIC_REG(NVIC_ICPR1_OFFSET) = (1 << (irq - 32));
    }
  else if (irq < 96)
    {
      NVIC_REG(NVIC_ICPR2_OFFSET) = (1 << (irq - 64));
    }
  else if (irq < 128)
    {
      NVIC_REG(NVIC_ICPR3_OFFSET) = (1 << (irq - 96));
    }
}
