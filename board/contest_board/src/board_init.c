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
#include <nuttx/spi/spi.h>
#include <nuttx/mtd/mtd.h>
#include <nuttx/spi/qspi.h>
#include <syslog.h>
#include <arch/board/board.h>

#include "stm32_spi.h"
#include "stm32_xspi.h"

/* MTD driver for MX25UM25645G */

FAR struct mtd_dev_s *mx25um25645g_initialize(FAR struct qspi_dev_s *qspi);

#ifdef CONFIG_THERMAL_PRINTER
#  include <nuttx/misc/thermal_printer.h>
#endif

#ifdef CONFIG_STM32N6_DCMIPP
extern int board_dcmipp_initialize(void);
#endif

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
  stm32_spibus_initialize(1);
#endif

#ifdef CONFIG_STM32N6_SPI2
  stm32_spibus_initialize(2);
#endif

#ifdef CONFIG_STM32N6_SPI3
  stm32_spibus_initialize(3);
#endif

  /* Register thermal printer on SPI1 */

#ifdef CONFIG_THERMAL_PRINTER
  {
    FAR struct spi_dev_s *spi = stm32_spibus_initialize(1);
    if (spi != NULL)
      {
        thermal_printer_register(spi);
      }
  }
#endif

  /* Initialize XSPI2 NOR Flash */

#ifdef CONFIG_STM32N6_XSPI2
  {
    FAR struct qspi_dev_s *xspi;
    FAR struct mtd_dev_s *mtd;

    xspi = stm32n6_xspi_initialize(2);
    if (xspi != NULL)
      {
        mtd = mx25um25645g_initialize(xspi);
        if (mtd != NULL)
          {
            /* Register the MTD device */

            syslog(LOG_NOTICE, "XSPI2 NOR Flash: MTD initialized\n");
          }
        else
          {
            syslog(LOG_ERR, "ERROR: Failed to initialize MTD\n");
          }
      }
    else
      {
        syslog(LOG_ERR, "ERROR: Failed to initialize XSPI2\n");
      }
  }
#endif

#ifdef CONFIG_STM32N6_DCMIPP
  /* Initialize DCMIPP camera interface and sensors */

  board_dcmipp_initialize();
#endif

  /* Initialize AI/TinyML system */

#ifdef CONFIG_AI
  extern int stm32_ai_init(void);
  stm32_ai_init();
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
