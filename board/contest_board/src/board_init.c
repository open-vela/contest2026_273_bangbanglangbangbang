/****************************************************************************
 * vendor/st/boards/stm32n647-evb/src/board_init.c
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
#include <nuttx/board.h>
#include <arch/board/board.h>

#include "stm32_spi.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_initialize
 *
 * Description:
 *   Board-specific initialization called after OS initialization.
 *
 ****************************************************************************/

void board_initialize(void)
{
  /* Configure system clock */

  /* Initialize GPIO */

  /* Initialize UART for console */

  /* Initialize SPI chip select pins */

#ifdef CONFIG_STM32N6_SPI1
  stm32_spi1initialize();
#endif

#ifdef CONFIG_STM32N6_SPI2
  stm32_spi2initialize();
#endif

#ifdef CONFIG_STM32N6_SPI3
  stm32_spi3initialize();
#endif

  /* Initialize other peripherals as needed */
}

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Application-specific initialization called by NSH.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARDCTL
int board_app_initialize(uintptr_t arg)
{
  /* Application-specific initialization */

  return 0;
}
#endif

/****************************************************************************
 * Name: board_autoled_on
 *
 * Description:
 *   Turn on an LED.
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_LEDS
void board_autoled_on(int led)
{
  /* TODO: Implement LED control */
  UNUSED(led);
}

/****************************************************************************
 * Name: board_autoled_off
 *
 * Description:
 *   Turn off an LED.
 *
 ****************************************************************************/

void board_autoled_off(int led)
{
  /* TODO: Implement LED control */
  UNUSED(led);
}
#endif
