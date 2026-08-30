/****************************************************************************
 * arch/arm/src/stm32n6/stm32_gpio.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_STM32_GPIO_H
#define __ARCH_ARM_SRC_STM32N6_STM32_GPIO_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/irq.h>

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Name: stm32_gpioinit
 *
 * Description:
 *   Initialize GPIO subsystem and configure default pins.
 *
 ****************************************************************************/

void stm32_gpioinit(void);

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
                      uint8_t af);

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

int stm32_gpio_write(uint8_t port, uint8_t pin, bool value);

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

int stm32_gpio_read(uint8_t port, uint8_t pin, bool *value);

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
                          xcpt_t handler, void *arg);

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

int stm32_gpio_irq_enable(uint8_t port, uint8_t pin);

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

int stm32_gpio_irq_disable(uint8_t port, uint8_t pin);

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

int stm32_configgpio(uint32_t pinset);

#ifdef __cplusplus
}
#endif

#endif /* __ARCH_ARM_SRC_STM32N6_STM32_GPIO_H */
