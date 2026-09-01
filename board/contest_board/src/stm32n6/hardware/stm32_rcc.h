/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_rcc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RCC register definitions for STM32N6xx
 * Based on STM32N647xx CMSIS headers (RM0486)
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_RCC_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_RCC_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* RCC Base Address */

#define STM32_RCC_BASE    0x44020C00

/* RCC Register Offsets - Clock Source Configuration */

#define RCC_CR_OFFSET           0x0000  /* Clock control register */
#define RCC_SR_OFFSET           0x0004  /* Status register */
#define RCC_STOPCR_OFFSET       0x0008  /* Stop mode control register */
#define RCC_CFGR1_OFFSET        0x0020  /* Clock configuration register 1 */
#define RCC_CFGR2_OFFSET        0x0024  /* Clock configuration register 2 */
#define RCC_BDCR_OFFSET         0x002C  /* Backup domain control register */
#define RCC_HWRSR_OFFSET        0x0030  /* Hardware reset status register */
#define RCC_RSR_OFFSET          0x0034  /* Reset register */

/* RCC Register Offsets - Oscillator Configuration */

#define RCC_LSECFGR_OFFSET      0x0040  /* LSE configuration register */
#define RCC_MSICFGR_OFFSET      0x0044  /* MSI configuration register */
#define RCC_HSICFGR_OFFSET      0x0048  /* HSI configuration register */
#define RCC_HSECFGR_OFFSET      0x0054  /* HSE configuration register */

/* RCC Register Offsets - PLL Configuration */

#define RCC_PLL1CFGR1_OFFSET    0x0080  /* PLL1 configuration register 1 */
#define RCC_PLL1CFGR2_OFFSET    0x0084  /* PLL1 configuration register 2 */
#define RCC_PLL1CFGR3_OFFSET    0x0088  /* PLL1 configuration register 3 */
#define RCC_PLL2CFGR1_OFFSET    0x0090  /* PLL2 configuration register 1 */
#define RCC_PLL2CFGR2_OFFSET    0x0094  /* PLL2 configuration register 2 */
#define RCC_PLL2CFGR3_OFFSET    0x0098  /* PLL2 configuration register 3 */
#define RCC_PLL3CFGR1_OFFSET    0x00A0  /* PLL3 configuration register 1 */
#define RCC_PLL3CFGR2_OFFSET    0x00A4  /* PLL3 configuration register 2 */
#define RCC_PLL3CFGR3_OFFSET    0x00A8  /* PLL3 configuration register 3 */
#define RCC_PLL4CFGR1_OFFSET    0x00B0  /* PLL4 configuration register 1 */
#define RCC_PLL4CFGR2_OFFSET    0x00B4  /* PLL4 configuration register 2 */
#define RCC_PLL4CFGR3_OFFSET    0x00B8  /* PLL4 configuration register 3 */

/* RCC Register Offsets - IC (Interconnect Clock) Configuration */

#define RCC_IC1CFGR_OFFSET      0x00C4  /* IC1 configuration register */
#define RCC_IC2CFGR_OFFSET      0x00C8  /* IC2 configuration register */
#define RCC_IC3CFGR_OFFSET      0x00CC  /* IC3 configuration register */
#define RCC_IC4CFGR_OFFSET      0x00D0  /* IC4 configuration register */
#define RCC_IC5CFGR_OFFSET      0x00D4  /* IC5 configuration register */
#define RCC_IC6CFGR_OFFSET      0x00D8  /* IC6 configuration register */
#define RCC_IC7CFGR_OFFSET      0x00DC  /* IC7 configuration register */
#define RCC_IC8CFGR_OFFSET      0x00E0  /* IC8 configuration register */
#define RCC_IC9CFGR_OFFSET      0x00E4  /* IC9 configuration register */
#define RCC_IC10CFGR_OFFSET     0x00E8  /* IC10 configuration register */
#define RCC_IC11CFGR_OFFSET     0x00EC  /* IC11 configuration register */
#define RCC_IC12CFGR_OFFSET     0x00F0  /* IC12 configuration register */
#define RCC_IC13CFGR_OFFSET     0x00F4  /* IC13 configuration register */
#define RCC_IC14CFGR_OFFSET     0x00F8  /* IC14 configuration register */
#define RCC_IC15CFGR_OFFSET     0x00FC  /* IC15 configuration register */
#define RCC_IC16CFGR_OFFSET     0x0100  /* IC16 configuration register */
#define RCC_IC17CFGR_OFFSET     0x0104  /* IC17 configuration register */
#define RCC_IC18CFGR_OFFSET     0x0108  /* IC18 configuration register */
#define RCC_IC19CFGR_OFFSET     0x010C  /* IC19 configuration register */
#define RCC_IC20CFGR_OFFSET     0x0110  /* IC20 configuration register */

/* RCC Register Offsets - Clock Independent Peripheral Configuration */

#define RCC_CCIPR1_OFFSET       0x0144  /* Clock config independent periph 1 */
#define RCC_CCIPR2_OFFSET       0x0148  /* Clock config independent periph 2 */
#define RCC_CCIPR3_OFFSET       0x014C  /* Clock config independent periph 3 */
#define RCC_CCIPR4_OFFSET       0x0150  /* Clock config independent periph 4 */
#define RCC_CCIPR5_OFFSET       0x0154  /* Clock config independent periph 5 */
#define RCC_CCIPR6_OFFSET       0x0158  /* Clock config independent periph 6 */
#define RCC_CCIPR7_OFFSET       0x015C  /* Clock config independent periph 7 */
#define RCC_CCIPR8_OFFSET       0x0160  /* Clock config independent periph 8 */
#define RCC_CCIPR9_OFFSET       0x0164  /* Clock config independent periph 9 */

/* RCC Register Offsets - Reset Registers */

#define RCC_AHB1RSTR_OFFSET     0x0210  /* AHB1 reset register */
#define RCC_AHB2RSTR_OFFSET     0x0214  /* AHB2 reset register */
#define RCC_AHB3RSTR_OFFSET     0x0218  /* AHB3 reset register */
#define RCC_AHB4RSTR_OFFSET     0x021C  /* AHB4 reset register */
#define RCC_AHB5RSTR_OFFSET     0x0220  /* AHB5 reset register */
#define RCC_APB1RSTR1_OFFSET    0x0224  /* APB1 reset register 1 */
#define RCC_APB1RSTR2_OFFSET    0x0228  /* APB1 reset register 2 */
#define RCC_APB2RSTR_OFFSET     0x022C  /* APB2 reset register */
#define RCC_APB4RSTR1_OFFSET    0x0234  /* APB4 reset register 1 */
#define RCC_APB4RSTR2_OFFSET    0x0238  /* APB4 reset register 2 */
#define RCC_APB5RSTR_OFFSET     0x023C  /* APB5 reset register */

/* RCC Register Offsets - Peripheral Clock Enable Registers (read/write) */

#define RCC_DIVENR_OFFSET       0x0240  /* IC dividers enable register */
#define RCC_BUSENR_OFFSET       0x0244  /* Embedded buses enable register */
#define RCC_MISCENR_OFFSET      0x0248  /* Miscellaneous enable register */
#define RCC_MEMENR_OFFSET       0x024C  /* Embedded memories enable register */
#define RCC_AHB1ENR_OFFSET      0x0250  /* AHB1 peripheral clock enable */
#define RCC_AHB2ENR_OFFSET      0x0254  /* AHB2 peripheral clock enable */
#define RCC_AHB3ENR_OFFSET      0x0258  /* AHB3 peripheral clock enable */
#define RCC_AHB4ENR_OFFSET      0x025C  /* AHB4 peripheral clock enable */
#define RCC_AHB5ENR_OFFSET      0x0260  /* AHB5 peripheral clock enable */
#define RCC_APB1ENR1_OFFSET     0x0264  /* APB1 peripheral clock enable 1 */
#define RCC_APB1ENR2_OFFSET     0x0268  /* APB1 peripheral clock enable 2 */
#define RCC_APB2ENR_OFFSET      0x026C  /* APB2 peripheral clock enable */
#define RCC_APB3ENR_OFFSET      0x0270  /* APB3 peripheral clock enable */
#define RCC_APB4ENR1_OFFSET     0x0274  /* APB4 peripheral clock enable 1 */
#define RCC_APB4ENR2_OFFSET     0x0278  /* APB4 peripheral clock enable 2 */
#define RCC_APB5ENR_OFFSET      0x027C  /* APB5 peripheral clock enable */

/* RCC Register Offsets - Low Power Clock Enable Registers */

#define RCC_AHB1LPENR_OFFSET    0x0290  /* AHB1 sleep enable register */
#define RCC_AHB2LPENR_OFFSET    0x0294  /* AHB2 sleep enable register */
#define RCC_AHB4LPENR_OFFSET    0x029C  /* AHB4 sleep enable register */
#define RCC_APB1LPENR1_OFFSET   0x02A4  /* APB1 sleep enable register 1 */
#define RCC_APB1LPENR2_OFFSET   0x02A8  /* APB1 sleep enable register 2 */
#define RCC_APB2LPENR_OFFSET    0x02AC  /* APB2 sleep enable register */
#define RCC_APB4LPENR1_OFFSET   0x02B4  /* APB4 sleep enable register 1 */
#define RCC_APB4LPENR2_OFFSET   0x02B8  /* APB4 sleep enable register 2 */
#define RCC_APB5LPENR_OFFSET    0x02BC  /* APB5 sleep enable register */

/* RCC Register Offsets - Reset Status Registers */

#define RCC_AHB1RSTSR_OFFSET    0x0A10  /* AHB1 reset status register */
#define RCC_AHB2RSTSR_OFFSET    0x0A14  /* AHB2 reset status register */
#define RCC_AHB3RSTSR_OFFSET    0x0A18  /* AHB3 reset status register */
#define RCC_AHB4RSTSR_OFFSET    0x0A1C  /* AHB4 reset status register */
#define RCC_AHB5RSTSR_OFFSET    0x0A20  /* AHB5 reset status register */
#define RCC_APB1RSTSR1_OFFSET   0x0A24  /* APB1 reset status register 1 */
#define RCC_APB1RSTSR2_OFFSET   0x0A28  /* APB1 reset status register 2 */
#define RCC_APB2RSTSR_OFFSET    0x0A2C  /* APB2 reset status register */
#define RCC_APB4RSTSR1_OFFSET   0x0A34  /* APB4 reset status register 1 */
#define RCC_APB4RSTSR2_OFFSET   0x0A38  /* APB4 reset status register 2 */
#define RCC_APB5RSTSR_OFFSET    0x0A3C  /* APB5 reset status register */

/* RCC Register Offsets - Enable Status Registers (read status / write to set) */

#define RCC_DIVENSR_OFFSET      0x0A40  /* Divider enable status register */
#define RCC_BUSENSR_OFFSET      0x0A44  /* Bus enable status register */
#define RCC_MISCENSR_OFFSET     0x0A48  /* Miscellaneous enable status register */
#define RCC_MEMENSR_OFFSET      0x0A4C  /* Memory enable status register */
#define RCC_AHB1ENSR_OFFSET     0x0A50  /* AHB1 enable status register */
#define RCC_AHB2ENSR_OFFSET     0x0A54  /* AHB2 enable status register */
#define RCC_AHB3ENSR_OFFSET     0x0A58  /* AHB3 enable status register */
#define RCC_AHB4ENSR_OFFSET     0x0A5C  /* AHB4 enable status register */
#define RCC_AHB5ENSR_OFFSET     0x0A60  /* AHB5 enable status register */
#define RCC_APB1ENSR1_OFFSET    0x0A64  /* APB1 enable status register 1 */
#define RCC_APB1ENSR2_OFFSET    0x0A68  /* APB1 enable status register 2 */
#define RCC_APB2ENSR_OFFSET     0x0A6C  /* APB2 enable status register */
#define RCC_APB3ENSR_OFFSET     0x0A70  /* APB3 enable status register */
#define RCC_APB4ENSR1_OFFSET    0x0A74  /* APB4 enable status register 1 */
#define RCC_APB4ENSR2_OFFSET    0x0A78  /* APB4 enable status register 2 */
#define RCC_APB5ENSR_OFFSET     0x0A7C  /* APB5 enable status register */

/* RCC_CR Register Bits */

#define RCC_CR_HSION            (1 << 0)   /* HSI clock enable */
#define RCC_CR_HSIRDY           (1 << 2)   /* HSI clock ready */
#define RCC_CR_HSEON            (1 << 16)  /* HSE clock enable */
#define RCC_CR_HSERDY           (1 << 17)  /* HSE clock ready */
#define RCC_CR_PLL1ON           (1 << 24)  /* PLL1 enable */
#define RCC_CR_PLL1RDY          (1 << 25)  /* PLL1 ready */

/* RCC_CFGR1 Register Bits */

#define RCC_CFGR1_SW_MASK       (0x3 << 0) /* System clock switch mask */
#define RCC_CFGR1_SW_HSI        (0x0 << 0) /* HSI as system clock */
#define RCC_CFGR1_SW_HSE        (0x1 << 0) /* HSE as system clock */
#define RCC_CFGR1_SW_PLL1       (0x3 << 0) /* PLL1 as system clock */

#define RCC_CFGR1_SWS_MASK      (0x3 << 2) /* System clock switch status mask */
#define RCC_CFGR1_SWS_HSI       (0x0 << 2) /* HSI used as system clock */
#define RCC_CFGR1_SWS_HSE       (1 << 2)   /* HSE used as system clock */
#define RCC_CFGR1_SWS_PLL1      (0x3 << 2) /* PLL1 used as system clock */

/* RCC_PLL1CFGR1 Register Bits */

#define RCC_PLL1CFGR1_PLL1SEL_MASK   (0x3 << 0)  /* PLL1 source mask */
#define RCC_PLL1CFGR1_PLL1SEL_HSI    (0x0 << 0)  /* HSI as PLL1 source */
#define RCC_PLL1CFGR1_PLL1SEL_HSE    (0x1 << 0)  /* HSE as PLL1 source */

#define RCC_PLL1CFGR1_PLL1DIVM_MASK  (0x3F << 4)  /* PLL1 M divider mask */
#define RCC_PLL1CFGR1_PLL1DIVN_MASK  (0x7FF << 8) /* PLL1 N multiplier mask */

/* Legacy PLL register definitions for backward compatibility */

#define RCC_PLL1CFGR_PLLSRC_HSI   RCC_PLL1CFGR1_PLL1SEL_HSI
#define RCC_PLL1CFGR_PLLSRC_HSE   RCC_PLL1CFGR1_PLL1SEL_HSE

/* Flash latency definitions */

#define FLASH_ACR_LATENCY_MASK  (0xF << 0)
#define FLASH_ACR_LATENCY_5WS   (0x5 << 0)  /* 5 wait states for 600MHz */

/****************************************************************************
 * Peripheral Clock Enable Bits
 ****************************************************************************/

/* AHB4 peripherals - GPIO (offset 0x025C) */

#define RCC_AHB4ENR_GPIOAEN    (1 << 0)   /* GPIOA clock enable */
#define RCC_AHB4ENR_GPIOBEN    (1 << 1)   /* GPIOB clock enable */
#define RCC_AHB4ENR_GPIOCEN    (1 << 2)   /* GPIOC clock enable */
#define RCC_AHB4ENR_GPIODEN    (1 << 3)   /* GPIOD clock enable */
#define RCC_AHB4ENR_GPIOEEN    (1 << 4)   /* GPIOE clock enable */
#define RCC_AHB4ENR_GPIOFEN    (1 << 5)   /* GPIOF clock enable */
#define RCC_AHB4ENR_GPIOGEN    (1 << 6)   /* GPIOG clock enable */
#define RCC_AHB4ENR_GPIOHEN    (1 << 7)   /* GPIOH clock enable */
#define RCC_AHB4ENR_GPIONEN    (1 << 13)  /* GPION clock enable */

/* AHB4 peripherals - PWR (offset 0x025C) */

#define RCC_AHB4ENR_PWREN      (1 << 18)  /* PWR clock enable */

/* AHB1 peripherals (offset 0x0250) */

#define RCC_AHB1ENR_GPDMA1EN   (1 << 4)   /* GPDMA1 clock enable */
#define RCC_AHB1ENR_ADC12EN    (1 << 5)   /* ADC12 clock enable */

/* AHB3 peripherals (offset 0x0258) */

#define RCC_AHB3ENR_RNGEN      (1 << 0)   /* RNG clock enable */
#define RCC_AHB3ENR_HASHEN     (1 << 1)   /* HASH clock enable */
#define RCC_AHB3ENR_PKAEN      (1 << 8)   /* PKA clock enable */

/* APB1 peripherals - register 1 (offset 0x0264) */

#define RCC_APB1ENR1_TIM2EN    (1 << 0)   /* TIM2 clock enable */
#define RCC_APB1ENR1_SPI2EN    (1 << 14)  /* SPI2 clock enable */
#define RCC_APB1ENR1_SPI3EN    (1 << 15)  /* SPI3 clock enable */
#define RCC_APB1ENR1_USART2EN  (1 << 17)  /* USART2 clock enable */
#define RCC_APB1ENR1_USART3EN  (1 << 18)  /* USART3 clock enable */
#define RCC_APB1ENR1_UART4EN   (1 << 19)  /* UART4 clock enable */
#define RCC_APB1ENR1_UART5EN   (1 << 20)  /* UART5 clock enable */
#define RCC_APB1ENR1_I2C1EN    (1 << 21)  /* I2C1 clock enable */
#define RCC_APB1ENR1_I2C2EN    (1 << 22)  /* I2C2 clock enable */
#define RCC_APB1ENR1_I2C3EN    (1 << 23)  /* I2C3 clock enable */
#define RCC_APB1ENR1_I3C1EN    (1 << 24)  /* I3C1 clock enable */
#define RCC_APB1ENR1_I3C2EN    (1 << 25)  /* I3C2 clock enable */
#define RCC_APB1ENR1_UART7EN   (1 << 30)  /* UART7 clock enable */
#define RCC_APB1ENR1_UART8EN   (1 << 31)  /* UART8 clock enable */

/* APB1 peripherals - register 2 (offset 0x0268) */

#define RCC_APB1ENR2_FDCANEN   (1 << 8)   /* FDCAN clock enable */

/* APB2 peripherals (offset 0x026C) */

#define RCC_APB2ENR_TIM1EN     (1 << 0)   /* TIM1 clock enable */
#define RCC_APB2ENR_TIM8EN     (1 << 1)   /* TIM8 clock enable */
#define RCC_APB2ENR_USART1EN   (1 << 4)   /* USART1 clock enable */
#define RCC_APB2ENR_USART6EN   (1 << 5)   /* USART6 clock enable */
#define RCC_APB2ENR_UART9EN    (1 << 6)   /* UART9 clock enable */
#define RCC_APB2ENR_USART10EN  (1 << 7)   /* USART10 clock enable */
#define RCC_APB2ENR_SPI1EN     (1 << 12)  /* SPI1 clock enable */
#define RCC_APB2ENR_SPI4EN     (1 << 13)  /* SPI4 clock enable */
#define RCC_APB2ENR_SPI5EN     (1 << 20)  /* SPI5 clock enable */
#define RCC_APB2ENR_SAI1EN     (1 << 21)  /* SAI1 clock enable */
#define RCC_APB2ENR_SAI2EN     (1 << 22)  /* SAI2 clock enable */

/* APB4 peripherals - register 1 (offset 0x0274) */

#define RCC_APB4ENR1_VREFBUFEN (1 << 15)  /* VREFBUF clock enable */

/* APB4 peripherals - register 2 (offset 0x0278) */

#define RCC_APB4ENR2_SYSCFGEN  (1 << 0)   /* SYSCFG clock enable */

/* APB5 peripherals (offset 0x027C) */

#define RCC_APB5ENR_LTDCEN     (1 << 1)   /* LTDC clock enable */

/* IC16CFGR register bits (offset 0x0100) - IC16 clock configuration */

#define RCC_IC16CFGR_IC16SRC_SHIFT   0     /* IC16 source selection [1:0] */
#define RCC_IC16CFGR_IC16SRC_MASK    (0x3 << RCC_IC16CFGR_IC16SRC_SHIFT)
#define RCC_IC16CFGR_IC16SRC_HSI     (0x0 << RCC_IC16CFGR_IC16SRC_SHIFT)
#define RCC_IC16CFGR_IC16SRC_HSE     (0x1 << RCC_IC16CFGR_IC16SRC_SHIFT)
#define RCC_IC16CFGR_IC16SRC_PLL1    (0x2 << RCC_IC16CFGR_IC16SRC_SHIFT)
#define RCC_IC16CFGR_IC16SRC_PLL4    (0x3 << RCC_IC16CFGR_IC16SRC_SHIFT)
#define RCC_IC16CFGR_IC16DIV_SHIFT   4     /* IC16 divider [6:4] */
#define RCC_IC16CFGR_IC16DIV_MASK    (0x7 << RCC_IC16CFGR_IC16DIV_SHIFT)

/* CCIPR5 register bits (offset 0x0154) - LTDC clock source selection */

#define RCC_CCIPR5_LTDCSEL_SHIFT    0     /* LTDC clock source [1:0] */
#define RCC_CCIPR5_LTDCSEL_MASK     (0x3 << RCC_CCIPR5_LTDCSEL_SHIFT)
#define RCC_CCIPR5_LTDCSEL_HSI      (0x0 << RCC_CCIPR5_LTDCSEL_SHIFT)
#define RCC_CCIPR5_LTDCSEL_HSE      (0x1 << RCC_CCIPR5_LTDCSEL_SHIFT)
#define RCC_CCIPR5_LTDCSEL_PLL4     (0x2 << RCC_CCIPR5_LTDCSEL_SHIFT)
#define RCC_CCIPR5_LTDCSEL_IC16     (0x3 << RCC_CCIPR5_LTDCSEL_SHIFT)

/* AHB5 peripherals (offset 0x0260) */

#define RCC_AHB5ENR_XSPI1EN    (1 << 0)   /* XSPI1 clock enable */
#define RCC_AHB5ENR_XSPI2EN    (1 << 1)   /* XSPI2 clock enable */
#define RCC_AHB5ENR_XSPI3EN    (1 << 2)   /* XSPI3 clock enable */
#define RCC_AHB5ENR_XSPIMEN    (1 << 3)   /* XSPI IO Manager clock enable */

/* AHB5 Reset register bits (offset 0x0220) */

#define RCC_AHB5RSTR_XSPI1RST  (1 << 0)   /* XSPI1 reset */
#define RCC_AHB5RSTR_XSPI2RST  (1 << 1)   /* XSPI2 reset */
#define RCC_AHB5RSTR_XSPI3RST  (1 << 2)   /* XSPI3 reset */

/* BUSENR register - enable all bus domains */

#define RCC_BUSENR_ALL         0xFFFFFFFF /* Enable all bus access */

/****************************************************************************
 * Legacy compatibility definitions (mapped to correct AHB4 for GPIO)
 ****************************************************************************/

/* For backward compatibility - GPIO is on AHB4 in STM32N6, not AHB2 */

#define RCC_AHB2ENR_GPIOAEN    RCC_AHB4ENR_GPIOAEN
#define RCC_AHB2ENR_GPIOBEN    RCC_AHB4ENR_GPIOBEN
#define RCC_AHB2ENR_GPIOCEN    RCC_AHB4ENR_GPIOCEN
#define RCC_AHB2ENR_GPIODEN    RCC_AHB4ENR_GPIODEN
#define RCC_AHB2ENR_GPIOEEN    RCC_AHB4ENR_GPIOEEN
#define RCC_AHB2ENR_GPIOFEN    RCC_AHB4ENR_GPIOFEN
#define RCC_AHB2ENR_GPIOGEN    RCC_AHB4ENR_GPIOGEN
#define RCC_AHB2ENR_GPIOHEN    RCC_AHB4ENR_GPIOHEN

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline uint32_t rcc_get_cr(void)
{
  return *(volatile uint32_t *)(STM32_RCC_BASE + RCC_CR_OFFSET);
}

static inline void rcc_set_cr(uint32_t value)
{
  *(volatile uint32_t *)(STM32_RCC_BASE + RCC_CR_OFFSET) = value;
}

static inline uint32_t rcc_get_cfgr1(void)
{
  return *(volatile uint32_t *)(STM32_RCC_BASE + RCC_CFGR1_OFFSET);
}

static inline void rcc_set_cfgr1(uint32_t value)
{
  *(volatile uint32_t *)(STM32_RCC_BASE + RCC_CFGR1_OFFSET) = value;
}

static inline uint32_t rcc_get_pll1cfgr(void)
{
  return *(volatile uint32_t *)(STM32_RCC_BASE + RCC_PLL1CFGR1_OFFSET);
}

static inline void rcc_set_pll1cfgr(uint32_t value)
{
  *(volatile uint32_t *)(STM32_RCC_BASE + RCC_PLL1CFGR1_OFFSET) = value;
}

static inline uint32_t rcc_get_pll1divr(void)
{
  return *(volatile uint32_t *)(STM32_RCC_BASE + RCC_PLL1CFGR3_OFFSET);
}

static inline void rcc_set_pll1divr(uint32_t value)
{
  *(volatile uint32_t *)(STM32_RCC_BASE + RCC_PLL1CFGR3_OFFSET) = value;
}

/* Bus clock enable function */

static inline void rcc_enable_bus_clocks(void)
{
  *(volatile uint32_t *)(STM32_RCC_BASE + RCC_BUSENR_OFFSET) =
    RCC_BUSENR_ALL;
}

/* AHB4 clock enable (GPIO, PWR) */

static inline void rcc_enable_ahb4_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB4ENR_OFFSET);
  *reg |= mask;
}

/* AHB2 clock enable (RAMCFG, MDF, ADF) */

static inline void rcc_enable_ahb2_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB2ENR_OFFSET);
  *reg |= mask;
}

/* APB2 clock enable (USART1, SPI1, SPI4, SAI1/2) */

static inline void rcc_enable_apb2_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_APB2ENR_OFFSET);
  *reg |= mask;
}

/* APB1 clock enable (USART2/3, UART4/5, I2C1/2/3, SPI2/3) */

static inline void rcc_enable_apb1_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_APB1ENR1_OFFSET);
  *reg |= mask;
}

/* APB5 clock enable (LTDC, DSI) */

static inline void rcc_enable_apb5_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_APB5ENR_OFFSET);
  *reg |= mask;
}

/* AHB5 clock enable (XSPI) */

static inline void rcc_enable_ahb5_clock(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB5ENR_OFFSET);
  *reg |= mask;
}

/* AHB5 reset functions */

static inline void rcc_set_ahb5_reset(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB5RSTR_OFFSET);
  *reg |= mask;
}

static inline void rcc_clear_ahb5_reset(uint32_t mask)
{
  volatile uint32_t *reg =
    (volatile uint32_t *)(STM32_RCC_BASE + RCC_AHB5RSTR_OFFSET);
  *reg &= ~mask;
}

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_RCC_H */
