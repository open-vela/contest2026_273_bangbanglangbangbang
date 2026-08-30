/****************************************************************************
 * arch/arm/src/stm32n6/stm32_gpio.c
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
#include <nuttx/irq.h>
#include <nuttx/spinlock.h>

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "arm_internal.h"
#include "hardware/stm32_gpio.h"
#include "hardware/stm32_nvic.h"
#include "hardware/stm32_rcc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* USART1 GPIO Pins (PA9=TX, PA10=RX) */

#define USART1_TX_PIN   9
#define USART1_RX_PIN   10

/* Maximum number of EXTI lines */

#define STM32_EXTI_LINES  16

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* GPIO interrupt handler entry */

struct gpio_irq_handler_s
{
  xcpt_t handler;       /* User interrupt handler */
  void *arg;            /* Argument for handler */
  uint8_t port;         /* GPIO port */
  uint8_t pin;          /* GPIO pin number */
  bool inuse;           /* True if entry is in use */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Table of GPIO interrupt handlers */

static struct gpio_irq_handler_s g_gpio_irq_handlers[STM32_EXTI_LINES];

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_gpio_get_base
 *
 * Description:
 *   Get GPIO base address from port ID.
 *
 * Input Parameters:
 *   port - GPIO port ID (GPIO_PORTA..GPIO_PORTH)
 *
 * Returned Value:
 *   GPIO base address or 0 on error
 *
 ****************************************************************************/

static uint32_t stm32_gpio_get_base(uint8_t port)
{
  switch (port)
    {
      case GPIO_PORTA:
        return STM32_GPIOA_BASE;
      case GPIO_PORTB:
        return STM32_GPIOB_BASE;
      case GPIO_PORTC:
        return STM32_GPIOC_BASE;
      case GPIO_PORTD:
        return STM32_GPIOD_BASE;
      case GPIO_PORTE:
        return STM32_GPIOE_BASE;
      case GPIO_PORTF:
        return STM32_GPIOF_BASE;
      case GPIO_PORTG:
        return STM32_GPIOG_BASE;
      case GPIO_PORTH:
        return STM32_GPIOH_BASE;
      default:
        return 0;
    }
}

/****************************************************************************
 * Name: stm32_exti_interrupt
 *
 * Description:
 *   Common EXTI interrupt handler. Dispatches to registered handlers.
 *
 ****************************************************************************/

static int stm32_exti_interrupt(int irq, void *context, void *arg)
{
  uint32_t pending;
  int i;

  /* Read pending interrupts */

  volatile uint32_t *rpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_RPR1_OFFSET);
  volatile uint32_t *fpr = (volatile uint32_t *)(STM32_EXTI_BASE + EXTI_FPR1_OFFSET);
  pending = *rpr | *fpr;

  /* Process each pending interrupt */

  for (i = 0; i < STM32_EXTI_LINES; i++)
    {
      if ((pending & (1 << i)) && g_gpio_irq_handlers[i].inuse)
        {
          /* Call registered handler */

          g_gpio_irq_handlers[i].handler(irq, context,
                                          g_gpio_irq_handlers[i].arg);

          /* Clear pending bit */

          exti_clear_pending(i);
        }
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_gpioinit
 *
 * Description:
 *   Initialize GPIO for USART1 console and other peripherals.
 *
 ****************************************************************************/

void stm32_gpioinit(void)
{
  int i;

  /* Enable GPIOA clock for USART1 pins */

  rcc_enable_ahb4_clock(RCC_AHB4ENR_GPIOAEN);

  /* Initialize IRQ handler table */

  for (i = 0; i < STM32_EXTI_LINES; i++)
    {
      g_gpio_irq_handlers[i].inuse = false;
    }

  /* Configure USART1 TX pin (PA9) as alternate function */

  gpio_set_mode(STM32_GPIOA_BASE, USART1_TX_PIN, GPIO_MODER_AF);
  gpio_set_af(STM32_GPIOA_BASE, USART1_TX_PIN, GPIO_AF7_USART1);
  gpio_set_speed(STM32_GPIOA_BASE, USART1_TX_PIN, GPIO_OSPEEDR_HIGH);
  gpio_set_output_type(STM32_GPIOA_BASE, USART1_TX_PIN, GPIO_OTYPER_PP);
  gpio_set_pupd(STM32_GPIOA_BASE, USART1_TX_PIN, GPIO_PUPDR_NONE);

  /* Configure USART1 RX pin (PA10) as alternate function */

  gpio_set_mode(STM32_GPIOA_BASE, USART1_RX_PIN, GPIO_MODER_AF);
  gpio_set_af(STM32_GPIOA_BASE, USART1_RX_PIN, GPIO_AF7_USART1);
  gpio_set_speed(STM32_GPIOA_BASE, USART1_RX_PIN, GPIO_OSPEEDR_HIGH);
  gpio_set_pupd(STM32_GPIOA_BASE, USART1_RX_PIN, GPIO_PUPDR_UP);
}

/****************************************************************************
 * Name: stm32_gpio_config
 *
 * Description:
 *   Configure a GPIO pin mode and properties.
 *
 * Input Parameters:
 *   port   - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin    - Pin number (0-15)
 *   mode   - GPIO mode (GPIO_MODER_INPUT, GPIO_MODER_OUTPUT, etc.)
 *   otype  - Output type (GPIO_OTYPER_PP or GPIO_OTYPER_OD)
 *   speed  - Output speed (GPIO_OSPEEDR_LOW..GPIO_OSPEEDR_VHIGH)
 *   pupd   - Pull-up/pull-down (GPIO_PUPDR_NONE, GPIO_PUPDR_UP, GPIO_PUPDR_DOWN)
 *   af     - Alternate function (only used if mode is GPIO_MODER_AF)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_config(uint8_t port, uint8_t pin, uint8_t mode,
                      uint8_t otype, uint8_t speed, uint8_t pupd,
                      uint8_t af)
{
  uint32_t base;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15)
    {
      return -EINVAL;
    }

  base = stm32_gpio_get_base(port);
  if (base == 0)
    {
      return -EINVAL;
    }

  /* Configure GPIO pin */

  gpio_set_mode(base, pin, mode);
  gpio_set_output_type(base, pin, otype);
  gpio_set_speed(base, pin, speed);
  gpio_set_pupd(base, pin, pupd);

  /* Set alternate function if needed */

  if (mode == GPIO_MODER_AF)
    {
      gpio_set_af(base, pin, af);
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_gpio_write
 *
 * Description:
 *   Write a value to a GPIO output pin.
 *
 * Input Parameters:
 *   port  - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin   - Pin number (0-15)
 *   value - Value to write (true=high, false=low)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_write(uint8_t port, uint8_t pin, bool value)
{
  uint32_t base;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15)
    {
      return -EINVAL;
    }

  base = stm32_gpio_get_base(port);
  if (base == 0)
    {
      return -EINVAL;
    }

  /* Set or clear pin */

  if (value)
    {
      gpio_set(base, pin);
    }
  else
    {
      gpio_clear(base, pin);
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_gpio_read
 *
 * Description:
 *   Read the current value of a GPIO pin.
 *
 * Input Parameters:
 *   port   - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin    - Pin number (0-15)
 *   value  - Pointer to store read value (true=high, false=low)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_read(uint8_t port, uint8_t pin, bool *value)
{
  uint32_t base;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15 || value == NULL)
    {
      return -EINVAL;
    }

  base = stm32_gpio_get_base(port);
  if (base == 0)
    {
      return -EINVAL;
    }

  /* Read pin value */

  *value = (gpio_read(base, pin) != 0);

  return OK;
}

/****************************************************************************
 * Name: stm32_gpio_irq_config
 *
 * Description:
 *   Configure GPIO interrupt for a pin.
 *
 * Input Parameters:
 *   port    - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin     - Pin number (0-15)
 *   trigger - Trigger type (EXTI_TRIGGER_RISING, EXTI_TRIGGER_FALLING,
 *             or EXTI_TRIGGER_BOTH)
 *   handler - Interrupt handler function
 *   arg     - Argument for handler
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_irq_config(uint8_t port, uint8_t pin, uint8_t trigger,
                          xcpt_t handler, void *arg)
{
  irqstate_t flags;
  int ret = OK;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15 || handler == NULL)
    {
      return -EINVAL;
    }

  /* Enter critical section */

  flags = enter_critical_section();

  /* Check if this EXTI line is already in use */

  if (g_gpio_irq_handlers[pin].inuse)
    {
      ret = -EBUSY;
      goto errout;
    }

  /* Register handler */

  g_gpio_irq_handlers[pin].handler = handler;
  g_gpio_irq_handlers[pin].arg = arg;
  g_gpio_irq_handlers[pin].port = port;
  g_gpio_irq_handlers[pin].pin = pin;
  g_gpio_irq_handlers[pin].inuse = true;

  /* Configure EXTI to use the specified port */

  exti_configure_port(pin, port);

  /* Configure trigger type */

  if (trigger & EXTI_TRIGGER_RISING)
    {
      exti_enable_rising_trigger(pin);
    }
  else
    {
      exti_disable_rising_trigger(pin);
    }

  if (trigger & EXTI_TRIGGER_FALLING)
    {
      exti_enable_falling_trigger(pin);
    }
  else
    {
      exti_disable_falling_trigger(pin);
    }

  /* Clear any pending interrupt */

  exti_clear_pending(pin);

  /* Enable EXTI interrupt */

  exti_enable_interrupt(pin);

  /* Attach EXTI interrupt handler if not already done */

  /* Note: EXTI lines 0-15 map to IRQ numbers that need to be configured
   * in the NVIC. The actual IRQ numbers depend on the chip implementation.
   * For STM32N6, EXTI lines share interrupts, so we use a common handler.
   */

  irq_attach(STM32_IRQ_EXTI0 + pin, stm32_exti_interrupt, NULL);

errout:
  leave_critical_section(flags);
  return ret;
}

/****************************************************************************
 * Name: stm32_gpio_irq_enable
 *
 * Description:
 *   Enable GPIO interrupt for a pin.
 *
 * Input Parameters:
 *   port - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin  - Pin number (0-15)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_irq_enable(uint8_t port, uint8_t pin)
{
  irqstate_t flags;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15)
    {
      return -EINVAL;
    }

  /* Enter critical section */

  flags = enter_critical_section();

  /* Check if handler is registered */

  if (!g_gpio_irq_handlers[pin].inuse ||
      g_gpio_irq_handlers[pin].port != port)
    {
      leave_critical_section(flags);
      return -EINVAL;
    }

  /* Enable EXTI interrupt */

  exti_enable_interrupt(pin);

  leave_critical_section(flags);
  return OK;
}

/****************************************************************************
 * Name: stm32_gpio_irq_disable
 *
 * Description:
 *   Disable GPIO interrupt for a pin.
 *
 * Input Parameters:
 *   port - GPIO port (GPIO_PORTA..GPIO_PORTH)
 *   pin  - Pin number (0-15)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_gpio_irq_disable(uint8_t port, uint8_t pin)
{
  irqstate_t flags;

  /* Validate parameters */

  if (port > GPIO_PORTH || pin > 15)
    {
      return -EINVAL;
    }

  /* Enter critical section */

  flags = enter_critical_section();

  /* Disable EXTI interrupt */

  exti_disable_interrupt(pin);

  leave_critical_section(flags);
  return OK;
}

/****************************************************************************
 * Name: stm32_configgpio
 *
 * Description:
 *   Configure a GPIO pin based on encoded pin configuration.
 *
 * Input Parameters:
 *   pinset - Encoded GPIO pin configuration (port | pin | af | flags)
 *
 * Returned Value:
 *   OK on success; negative errno on failure
 *
 ****************************************************************************/

int stm32_configgpio(uint32_t pinset)
{
  uint8_t port;
  uint8_t pin;
  uint8_t af;
  uint32_t base;

  /* Extract port, pin, and alternate function from pinset */

  port = (pinset >> 0) & 0x7;
  pin  = (pinset >> 3) & 0xf;
  af   = (pinset >> 7) & 0xf;

  /* Get GPIO base address */

  base = stm32_gpio_get_base(port);
  if (base == 0)
    {
      return -EINVAL;
    }

  /* Configure pin as alternate function */

  gpio_set_mode(base, pin, GPIO_MODER_AF);
  gpio_set_af(base, pin, af);
  gpio_set_speed(base, pin, GPIO_OSPEEDR_HIGH);
  gpio_set_pupd(base, pin, GPIO_PUPDR_NONE);

  return OK;
}
