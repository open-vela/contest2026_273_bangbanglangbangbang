/****************************************************************************
 * arch/arm/src/stm32n6/hardware/stm32_gpio.h
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_GPIO_H
#define __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_GPIO_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* GPIO Base Addresses */

#define STM32_GPIOA_BASE  0x42020000
#define STM32_GPIOB_BASE  0x42020400
#define STM32_GPIOC_BASE  0x42020800
#define STM32_GPIOD_BASE  0x42020C00
#define STM32_GPIOE_BASE  0x42021000
#define STM32_GPIOF_BASE  0x42021400
#define STM32_GPIOG_BASE  0x42021800
#define STM32_GPIOH_BASE  0x42021C00

/* GPIO Register Offsets */

#define GPIO_MODER_OFFSET   0x00  /* Mode register */
#define GPIO_OTYPER_OFFSET  0x04  /* Output type register */
#define GPIO_OSPEEDR_OFFSET 0x08  /* Output speed register */
#define GPIO_PUPDR_OFFSET   0x0C  /* Pull-up/pull-down register */
#define GPIO_IDR_OFFSET     0x10  /* Input data register */
#define GPIO_ODR_OFFSET     0x14  /* Output data register */
#define GPIO_BSRR_OFFSET    0x18  /* Bit set/reset register */
#define GPIO_LCKR_OFFSET    0x1C  /* Lock register */
#define GPIO_AFRL_OFFSET    0x20  /* Alternate function low register */
#define GPIO_AFRH_OFFSET    0x24  /* Alternate function high register */

/* GPIO Mode Bits */

#define GPIO_MODER_INPUT    0x0  /* Input mode */
#define GPIO_MODER_OUTPUT   0x1  /* Output mode */
#define GPIO_MODER_AF       0x2  /* Alternate function mode */
#define GPIO_MODER_ANALOG   0x3  /* Analog mode */

/* GPIO Output Type Bits */

#define GPIO_OTYPER_PP      0x0  /* Push-pull */
#define GPIO_OTYPER_OD      0x1  /* Open-drain */

/* GPIO Output Speed Bits */

#define GPIO_OSPEEDR_LOW    0x0  /* Low speed */
#define GPIO_OSPEEDR_MED    0x1  /* Medium speed */
#define GPIO_OSPEEDR_HIGH   0x2  /* High speed */
#define GPIO_OSPEEDR_VHIGH  0x3  /* Very high speed */

/* GPIO Pull-up/Pull-down Bits */

#define GPIO_PUPDR_NONE     0x0  /* No pull-up/pull-down */
#define GPIO_PUPDR_UP       0x1  /* Pull-up */
#define GPIO_PUPDR_DOWN     0x2  /* Pull-down */

/* GPIO Alternate Functions */

#define GPIO_AF0            0x0
#define GPIO_AF1            0x1
#define GPIO_AF2            0x2
#define GPIO_AF3            0x3
#define GPIO_AF4            0x4
#define GPIO_AF5            0x5
#define GPIO_AF6            0x6
#define GPIO_AF7            0x7
#define GPIO_AF8            0x8
#define GPIO_AF9            0x9
#define GPIO_AF10           0xa
#define GPIO_AF11           0xb
#define GPIO_AF12           0xc
#define GPIO_AF13           0xd
#define GPIO_AF14           0xe
#define GPIO_AF15           0xf

/* USART1 Alternate Function (AF7 for most STM32) */

#define GPIO_AF7_USART1     0x7

/* LTDC Alternate Functions */

#define GPIO_AF14_LTDC      GPIO_AF14
#define GPIO_AF10_LTDC      GPIO_AF10

/* I2C Alternate Function (AF4 for all I2C peripherals) */

#define GPIO_AF4_I2C        0x4

/* SPI Alternate Functions */

#define GPIO_AF5_SPI1       0x5
#define GPIO_AF5_SPI2       0x5
#define GPIO_AF5_SPI3       0x5

/* XSPI2 Alternate Function (AF9 for XSPI2 on Port G) */

#define GPIO_AF9_XSPI2      0x9

/* SPI1 Pin Definitions (example - actual pins depend on board) */

#define GPIO_SPI1_SCK       (GPIO_PORTA | GPIO_PIN5  | GPIO_AF5_SPI1)
#define GPIO_SPI1_MISO      (GPIO_PORTA | GPIO_PIN6  | GPIO_AF5_SPI1)
#define GPIO_SPI1_MOSI      (GPIO_PORTA | GPIO_PIN7  | GPIO_AF5_SPI1)

/* SPI2 Pin Definitions (example) */

#define GPIO_SPI2_SCK       (GPIO_PORTB | GPIO_PIN13 | GPIO_AF5_SPI2)
#define GPIO_SPI2_MISO      (GPIO_PORTB | GPIO_PIN14 | GPIO_AF5_SPI2)
#define GPIO_SPI2_MOSI      (GPIO_PORTB | GPIO_PIN15 | GPIO_AF5_SPI2)

/* SPI3 Pin Definitions (PB3=SCK, PB4=MISO, PB5=MOSI, AF6) */

#define GPIO_AF6_SPI3       0x6
#define GPIO_SPI3_SCK       (GPIO_PORTB | GPIO_PIN3  | GPIO_AF6_SPI3)
#define GPIO_SPI3_MISO      (GPIO_PORTB | GPIO_PIN4  | GPIO_AF6_SPI3)
#define GPIO_SPI3_MOSI      (GPIO_PORTB | GPIO_PIN5  | GPIO_AF6_SPI3)

/* EXTI Base Address */

#define STM32_EXTI_BASE     0x44022000

/* EXTI Register Offsets */

#define EXTI_RTSR1_OFFSET   0x00  /* Rising trigger selection register 1 */
#define EXTI_FTSR1_OFFSET   0x04  /* Falling trigger selection register 1 */
#define EXTI_SWIER1_OFFSET  0x08  /* Software interrupt event register 1 */
#define EXTI_RPR1_OFFSET    0x0C  /* Rising pending register 1 */
#define EXTI_FPR1_OFFSET    0x10  /* Falling pending register 1 */
#define EXTI_EXTICR1_OFFSET 0x60  /* External interrupt configuration register 1 */
#define EXTI_EXTICR2_OFFSET 0x64  /* External interrupt configuration register 2 */
#define EXTI_EXTICR3_OFFSET 0x68  /* External interrupt configuration register 3 */
#define EXTI_EXTICR4_OFFSET 0x6C  /* External interrupt configuration register 4 */
#define EXTI_IMR1_OFFSET    0x80  /* Interrupt mask register 1 */
#define EXTI_EMR1_OFFSET    0x84  /* Event mask register 1 */

/* EXTI Trigger Types */

#define EXTI_TRIGGER_RISING   0x01
#define EXTI_TRIGGER_FALLING  0x02
#define EXTI_TRIGGER_BOTH     0x03

/* GPIO Port IDs for EXTI configuration */

#define GPIO_PORTA  0
#define GPIO_PORTB  1
#define GPIO_PORTC  2
#define GPIO_PORTD  3
#define GPIO_PORTE  4
#define GPIO_PORTF  5
#define GPIO_PORTG  6
#define GPIO_PORTH  7
#define GPIO_PORTI  8
#define GPIO_PORTJ  9
#define GPIO_PORTK  10
#define GPIO_PORTL  11
#define GPIO_PORTM  12
#define GPIO_PORTN  13
#define GPIO_PORTO  14
#define GPIO_PORTP  15
#define GPIO_PORTQ  16

/* GPIO Pin Numbers */

#define GPIO_PIN0   0
#define GPIO_PIN1   1
#define GPIO_PIN2   2
#define GPIO_PIN3   3
#define GPIO_PIN4   4
#define GPIO_PIN5   5
#define GPIO_PIN6   6
#define GPIO_PIN7   7
#define GPIO_PIN8   8
#define GPIO_PIN9   9
#define GPIO_PIN10  10
#define GPIO_PIN11  11
#define GPIO_PIN12  12
#define GPIO_PIN13  13
#define GPIO_PIN14  14
#define GPIO_PIN15  15

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline void gpio_set_mode(uint32_t base, uint8_t pin, uint8_t mode)
{
  volatile uint32_t *moder = (volatile uint32_t *)(base + GPIO_MODER_OFFSET);
  uint32_t pos = pin * 2;
  uint32_t mask = 0x3 << pos;
  *moder = (*moder & ~mask) | ((mode & 0x3) << pos);
}

static inline void gpio_set_output_type(uint32_t base, uint8_t pin, uint8_t type)
{
  volatile uint32_t *otyper = (volatile uint32_t *)(base + GPIO_OTYPER_OFFSET);
  uint32_t mask = 1 << pin;
  if (type)
    {
      *otyper |= mask;
    }
  else
    {
      *otyper &= ~mask;
    }
}

static inline void gpio_set_speed(uint32_t base, uint8_t pin, uint8_t speed)
{
  volatile uint32_t *ospeedr = (volatile uint32_t *)(base + GPIO_OSPEEDR_OFFSET);
  uint32_t pos = pin * 2;
  uint32_t mask = 0x3 << pos;
  *ospeedr = (*ospeedr & ~mask) | ((speed & 0x3) << pos);
}

static inline void gpio_set_pupd(uint32_t base, uint8_t pin, uint8_t pupd)
{
  volatile uint32_t *pupdr = (volatile uint32_t *)(base + GPIO_PUPDR_OFFSET);
  uint32_t pos = pin * 2;
  uint32_t mask = 0x3 << pos;
  *pupdr = (*pupdr & ~mask) | ((pupd & 0x3) << pos);
}

static inline void gpio_set_af(uint32_t base, uint8_t pin, uint8_t af)
{
  volatile uint32_t *afr;
  if (pin < 8)
    {
      afr = (volatile uint32_t *)(base + GPIO_AFRL_OFFSET);
      *afr = (*afr & ~(0xF << (pin * 4))) | (af << (pin * 4));
    }
  else
    {
      afr = (volatile uint32_t *)(base + GPIO_AFRH_OFFSET);
      pin -= 8;
      *afr = (*afr & ~(0xF << (pin * 4))) | (af << (pin * 4));
    }
}

static inline void gpio_set(uint32_t base, uint8_t pin)
{
  volatile uint32_t *bsrr = (volatile uint32_t *)(base + GPIO_BSRR_OFFSET);
  *bsrr = (1 << pin);
}

static inline void gpio_clear(uint32_t base, uint8_t pin)
{
  volatile uint32_t *bsrr = (volatile uint32_t *)(base + GPIO_BSRR_OFFSET);
  *bsrr = (1 << (pin + 16));
}

static inline uint8_t gpio_read(uint32_t base, uint8_t pin)
{
  volatile uint32_t *idr = (volatile uint32_t *)(base + GPIO_IDR_OFFSET);
  return (*idr >> pin) & 0x1;
}

/* EXTI Configuration Functions */

static inline void exti_configure_port(uint8_t pin, uint8_t port)
{
  volatile uint32_t *exticr;
  uint8_t reg = pin / 4;
  uint8_t pos = (pin % 4) * 8;

  exticr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_EXTICR1_OFFSET + (reg * 4));
  *exticr = (*exticr & ~(0xFF << pos)) | (port << pos);
}

static inline void exti_enable_rising_trigger(uint8_t pin)
{
  volatile uint32_t *rtsr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_RTSR1_OFFSET);
  *rtsr |= (1 << pin);
}

static inline void exti_disable_rising_trigger(uint8_t pin)
{
  volatile uint32_t *rtsr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_RTSR1_OFFSET);
  *rtsr &= ~(1 << pin);
}

static inline void exti_enable_falling_trigger(uint8_t pin)
{
  volatile uint32_t *ftsr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_FTSR1_OFFSET);
  *ftsr |= (1 << pin);
}

static inline void exti_disable_falling_trigger(uint8_t pin)
{
  volatile uint32_t *ftsr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_FTSR1_OFFSET);
  *ftsr &= ~(1 << pin);
}

static inline void exti_enable_interrupt(uint8_t pin)
{
  volatile uint32_t *imr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_IMR1_OFFSET);
  *imr |= (1 << pin);
}

static inline void exti_disable_interrupt(uint8_t pin)
{
  volatile uint32_t *imr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_IMR1_OFFSET);
  *imr &= ~(1 << pin);
}

static inline uint32_t exti_get_pending(uint8_t pin)
{
  volatile uint32_t *rpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_RPR1_OFFSET);
  volatile uint32_t *fpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_FPR1_OFFSET);
  return ((*rpr | *fpr) >> pin) & 0x1;
}

static inline void exti_clear_pending(uint8_t pin)
{
  volatile uint32_t *rpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_RPR1_OFFSET);
  volatile uint32_t *fpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_FPR1_OFFSET);
  *rpr = (1 << pin);
  *fpr = (1 << pin);
}

#endif /* __ARCH_ARM_SRC_STM32N6_HARDWARE_STM32_GPIO_H */
