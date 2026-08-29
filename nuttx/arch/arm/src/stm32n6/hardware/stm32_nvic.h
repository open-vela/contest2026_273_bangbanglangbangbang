/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_nvic.h
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_NVIC_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_NVIC_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* NVIC Base Address (ARM Cortex-M standard) */

#define NVIC_BASE       0xE000E100
#define SCB_BASE        0xE000ED00
#define SYSTICK_BASE    0xE000E010

/* IRQ Numbers (STM32N6) */

#define STM32_IRQ_EXTI0       6   /* EXTI Line0 interrupt */
#define STM32_IRQ_EXTI1       7   /* EXTI Line1 interrupt */
#define STM32_IRQ_EXTI2       8   /* EXTI Line2 interrupt */
#define STM32_IRQ_EXTI3       9   /* EXTI Line3 interrupt */
#define STM32_IRQ_EXTI4      10   /* EXTI Line4 interrupt */
#define STM32_IRQ_EXTI5      23   /* EXTI Line5 interrupt */
#define STM32_IRQ_EXTI6      23   /* EXTI Line6 interrupt */
#define STM32_IRQ_EXTI7      23   /* EXTI Line7 interrupt */
#define STM32_IRQ_EXTI8      23   /* EXTI Line8 interrupt */
#define STM32_IRQ_EXTI9      23   /* EXTI Line9 interrupt */
#define STM32_IRQ_EXTI10     40   /* EXTI Line10 interrupt */
#define STM32_IRQ_EXTI11     40   /* EXTI Line11 interrupt */
#define STM32_IRQ_EXTI12     40   /* EXTI Line12 interrupt */
#define STM32_IRQ_EXTI13     40   /* EXTI Line13 interrupt */
#define STM32_IRQ_EXTI14     40   /* EXTI Line14 interrupt */
#define STM32_IRQ_EXTI15     40   /* EXTI Line15 interrupt */

#define STM32N6_IRQ_I2C1      31  /* I2C1 event interrupt */
#define STM32N6_IRQ_I2C1_ERR  32  /* I2C1 error interrupt */
#define STM32N6_IRQ_I2C2      33  /* I2C2 event interrupt */
#define STM32N6_IRQ_I2C2_ERR  34  /* I2C2 error interrupt */
#define STM32N6_IRQ_I2C3      72  /* I2C3 event interrupt */
#define STM32N6_IRQ_I2C3_ERR  73  /* I2C3 error interrupt */
#define STM32N6_IRQ_I2C4      95  /* I2C4 event interrupt */
#define STM32N6_IRQ_I2C4_ERR  96  /* I2C4 error interrupt */

/* NVIC Register Offsets */

#define NVIC_ISER0_OFFSET   0x000  /* Interrupt Set Enable Register 0 */
#define NVIC_ISER1_OFFSET   0x004  /* Interrupt Set Enable Register 1 */
#define NVIC_ISER2_OFFSET   0x008  /* Interrupt Set Enable Register 2 */
#define NVIC_ISER3_OFFSET   0x00C  /* Interrupt Set Enable Register 3 */
#define NVIC_ICER0_OFFSET   0x080  /* Interrupt Clear Enable Register 0 */
#define NVIC_ICER1_OFFSET   0x084  /* Interrupt Clear Enable Register 1 */
#define NVIC_ICER2_OFFSET   0x088  /* Interrupt Clear Enable Register 2 */
#define NVIC_ICER3_OFFSET   0x08C  /* Interrupt Clear Enable Register 3 */
#define NVIC_ISPR0_OFFSET   0x100  /* Interrupt Set Pending Register 0 */
#define NVIC_ISPR1_OFFSET   0x104  /* Interrupt Set Pending Register 1 */
#define NVIC_ISPR2_OFFSET   0x108  /* Interrupt Set Pending Register 2 */
#define NVIC_ISPR3_OFFSET   0x10C  /* Interrupt Set Pending Register 3 */
#define NVIC_ICPR0_OFFSET   0x180  /* Interrupt Clear Pending Register 0 */
#define NVIC_ICPR1_OFFSET   0x184  /* Interrupt Clear Pending Register 1 */
#define NVIC_ICPR2_OFFSET   0x188  /* Interrupt Clear Pending Register 2 */
#define NVIC_ICPR3_OFFSET   0x18C  /* Interrupt Clear Pending Register 3 */
#define NVIC_IABR0_OFFSET   0x200  /* Interrupt Active Bit Register 0 */
#define NVIC_IABR1_OFFSET   0x204  /* Interrupt Active Bit Register 1 */
#define NVIC_IABR2_OFFSET   0x208  /* Interrupt Active Bit Register 2 */
#define NVIC_IABR3_OFFSET   0x20C  /* Interrupt Active Bit Register 3 */
#define NVIC_IPR0_OFFSET    0x300  /* Interrupt Priority Register 0 */

/* SCB Register Offsets */

#define SCB_VTOR_OFFSET     0x008  /* Vector Table Offset Register */
#define SCB_AIRCR_OFFSET    0x00C  /* Application Interrupt and Reset Control Register */
#define SCB_SHPR1_OFFSET    0x018  /* System Handler Priority Register 1 */
#define SCB_SHPR2_OFFSET    0x01C  /* System Handler Priority Register 2 */
#define SCB_SHPR3_OFFSET    0x020  /* System Handler Priority Register 3 */

/* SysTick Register Offsets */

#define SYSTICK_CSR_OFFSET  0x00   /* Control and Status Register */
#define SYSTICK_RVR_OFFSET  0x04   /* Reload Value Register */
#define SYSTICK_CVR_OFFSET  0x08   /* Current Value Register */
#define SYSTICK_CALIB_OFFSET 0x0C  /* Calibration Register */

/* SysTick CSR Bits */

#define SYSTICK_CSR_ENABLE    (1 << 0)  /* Counter enable */
#define SYSTICK_CSR_TICKINT   (1 << 1)  /* Interrupt enable */
#define SYSTICK_CSR_CLKSOURCE (1 << 2)  /* Clock source: 1=processor, 0=external */
#define SYSTICK_CSR_COUNTFLAG (1 << 16) /* Count flag */

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline void nvic_enable_irq(int irq)
{
  volatile uint32_t *nvic_iser = (volatile uint32_t *)(NVIC_BASE + NVIC_ISER0_OFFSET + (irq / 32) * 4);
  *nvic_iser = (1 << (irq % 32));
}

static inline void nvic_disable_irq(int irq)
{
  volatile uint32_t *nvic_icer = (volatile uint32_t *)(NVIC_BASE + NVIC_ICER0_OFFSET + (irq / 32) * 4);
  *nvic_icer = (1 << (irq % 32));
}

static inline void nvic_set_priority(int irq, uint8_t priority)
{
  volatile uint8_t *nvic_ipr = (volatile uint8_t *)(NVIC_BASE + NVIC_IPR0_OFFSET + irq);
  *nvic_ipr = priority;
}

static inline void nvic_set_pending(int irq)
{
  volatile uint32_t *nvic_ispr = (volatile uint32_t *)(NVIC_BASE + NVIC_ISPR0_OFFSET + (irq / 32) * 4);
  *nvic_ispr = (1 << (irq % 32));
}

static inline void nvic_clear_pending(int irq)
{
  volatile uint32_t *nvic_icpr = (volatile uint32_t *)(NVIC_BASE + NVIC_ICPR0_OFFSET + (irq / 32) * 4);
  *nvic_icpr = (1 << (irq % 32));
}

static inline void scb_set_vtor(uint32_t vtor)
{
  *(volatile uint32_t *)(SCB_BASE + SCB_VTOR_OFFSET) = vtor;
}

static inline uint32_t systick_get_csr(void)
{
  return *(volatile uint32_t *)(SYSTICK_BASE + SYSTICK_CSR_OFFSET);
}

static inline void systick_set_csr(uint32_t value)
{
  *(volatile uint32_t *)(SYSTICK_BASE + SYSTICK_CSR_OFFSET) = value;
}

static inline void systick_set_rvr(uint32_t value)
{
  *(volatile uint32_t *)(SYSTICK_BASE + SYSTICK_RVR_OFFSET) = value;
}

static inline void systick_set_cvr(uint32_t value)
{
  *(volatile uint32_t *)(SYSTICK_BASE + SYSTICK_CVR_OFFSET) = value;
}

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_NVIC_H */
