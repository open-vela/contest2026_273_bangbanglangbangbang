/****************************************************************************
 * arch/arm/src/stm32n6/stm32_serial.c
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
#include <nuttx/serial/serial.h>

#include "arm_internal.h"

/****************************************************************************
 * External Definitions
 ****************************************************************************/

/* Console UART operations (defined in uart driver) */

extern struct uart_dev_s g_uart1port;

#ifdef CONFIG_STM32N6_USART2
extern struct uart_dev_s g_uart2port;
#endif

#ifdef CONFIG_STM32N6_USART3
extern struct uart_dev_s g_uart3port;
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_serialinit
 *
 * Description:
 *   Initialize the serial console.
 *
 ****************************************************************************/

void arm_serialinit(void)
{
  /* Register the console UART */

  uart_register("/dev/console", &g_uart1port);

#ifdef CONFIG_STM32N6_USART2
  uart_register("/dev/ttyS1", &g_uart2port);
#endif

#ifdef CONFIG_STM32N6_USART3
  uart_register("/dev/ttyS2", &g_uart3port);
#endif
}
