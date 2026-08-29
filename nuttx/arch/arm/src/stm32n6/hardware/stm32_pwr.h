/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_pwr.h
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_PWR_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_PWR_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* PWR Base Address */

#define STM32_PWR_BASE    0x44020800

/* PWR Register Offsets */

#define PWR_CR1_OFFSET      0x00  /* Power control register 1 */
#define PWR_CR3_OFFSET      0x08  /* Power control register 3 */
#define PWR_VOSCR_OFFSET    0x20  /* Voltage scaling control register */

/* PWR_CR1 Register Bits */

#define PWR_CR1_VOS_MASK    (0x3 << 9)  /* Voltage scaling mask */
#define PWR_CR1_VOS_SCALE1  (0x1 << 9)  /* Scale 1 (highest performance) */
#define PWR_CR1_VOS_SCALE2  (0x2 << 9)  /* Scale 2 */
#define PWR_CR1_VOS_SCALE3  (0x3 << 9)  /* Scale 3 (lowest power) */

/* PWR_CR3 Register Bits */

#define PWR_CR3_BYPASS      (1 << 0)    /* External supply mode */
#define PWR_CR3_LDOEN       (1 << 1)    /* LDO supply mode */
#define PWR_CR3_EN_VDDIO2   (1 << 9)    /* Enable VDDIO2 (GPIOG/H) */
#define PWR_CR3_EN_VDDIO3   (1 << 10)   /* Enable VDDIO3 (GPIOC/D) */
#define PWR_CR3_EN_VDDIO4   (1 << 11)   /* Enable VDDIO4 (GPIOE/F) */
#define PWR_CR3_EN_VDDIO5   (1 << 12)   /* Enable VDDIO5 (GPIOA/B) */

/* PWR_VOSCR Register Bits */

#define PWR_VOSCR_VOS       (1 << 0)    /* Voltage scaling selection */
#define PWR_VOSCR_VOSRDY    (1 << 1)    /* VOS ready flag */
#define PWR_VOSCR_ACTVOS    (1 << 16)   /* Current voltage scaling level */
#define PWR_VOSCR_ACTVOSRDY (1 << 17)   /* ACTVOS voltage level ready */

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_PWR_H */
