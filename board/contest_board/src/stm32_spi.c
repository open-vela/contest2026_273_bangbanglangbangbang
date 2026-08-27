/****************************************************************************
 * boards/arm/stm32n6/stm32n647-evb/src/stm32_spi.c
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
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#include <nuttx/spi/spi.h>

#include "arm_internal.h"
#include "stm32_gpio.h"
#include "stm32_spi.h"

#include <arch/board/board.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_spi1select
 *
 * Description:
 *   Select or deselect SPI1 device for active chip select.
 *   This function is called by the SPI driver to control the CS pin.
 *
 * Input Parameters:
 *   dev    - SPI device (unused)
 *   devid  - Device ID (unused for single device)
 *   selected - true to select (CS low), false to deselect (CS high)
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI1
void stm32_spi1select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  /* Active low chip select: selected = CS low, deselected = CS high */

  spiinfo("SPI1 CS %s (devid=%" PRIu32 ")\n",
          selected ? "SELECTED" : "DESELECTED", devid);

  stm32_gpio_write(SPI1_CS_PORT, SPI1_CS_PIN, !selected);
}

/****************************************************************************
 * Name: stm32_spi1status
 *
 * Description:
 *   Return SPI1 device status.
 *
 * Input Parameters:
 *   dev   - SPI device (unused)
 *   devid - Device ID (unused)
 *
 * Returned Value:
 *   Always returns 0 (no status bits set)
 *
 ****************************************************************************/

uint8_t stm32_spi1status(FAR struct spi_dev_s *dev, uint32_t devid)
{
  return 0;
}

#  ifdef CONFIG_SPI_CMDDATA
/****************************************************************************
 * Name: stm32_spi1cmddata
 *
 * Description:
 *   Select or deselect SPI1 command/data mode.
 *   Not used for standard SPI; provided for completeness.
 *
 ****************************************************************************/

int stm32_spi1cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
  return -ENODEV;
}
#  endif
#endif /* CONFIG_STM32N6_SPI1 */

/****************************************************************************
 * Name: stm32_spi2select
 *
 * Description:
 *   Select or deselect SPI2 device for active chip select.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI2
void stm32_spi2select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  spiinfo("SPI2 CS %s (devid=%" PRIu32 ")\n",
          selected ? "SELECTED" : "DESELECTED", devid);

  stm32_gpio_write(SPI2_CS_PORT, SPI2_CS_PIN, !selected);
}

/****************************************************************************
 * Name: stm32_spi2status
 *
 * Description:
 *   Return SPI2 device status.
 *
 ****************************************************************************/

uint8_t stm32_spi2status(FAR struct spi_dev_s *dev, uint32_t devid)
{
  return 0;
}

#  ifdef CONFIG_SPI_CMDDATA
int stm32_spi2cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
  return -ENODEV;
}
#  endif
#endif /* CONFIG_STM32N6_SPI2 */

/****************************************************************************
 * Name: stm32_spi3select
 *
 * Description:
 *   Select or deselect SPI3 device for active chip select.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI3
void stm32_spi3select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  spiinfo("SPI3 CS %s (devid=%" PRIu32 ")\n",
          selected ? "SELECTED" : "DESELECTED", devid);

  stm32_gpio_write(SPI3_CS_PORT, SPI3_CS_PIN, !selected);
}

/****************************************************************************
 * Name: stm32_spi3status
 *
 * Description:
 *   Return SPI3 device status.
 *
 ****************************************************************************/

uint8_t stm32_spi3status(FAR struct spi_dev_s *dev, uint32_t devid)
{
  return 0;
}

#  ifdef CONFIG_SPI_CMDDATA
int stm32_spi3cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
  return -ENODEV;
}
#  endif
#endif /* CONFIG_STM32N6_SPI3 */

/****************************************************************************
 * Name: stm32_spi1initialize / stm32_spi2initialize / stm32_spi3initialize
 *
 * Description:
 *   Initialize SPI CS GPIO pins. Called during board initialization.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_SPI1
void stm32_spi1initialize(void)
{
  /* Configure CS pin as GPIO output, push-pull, high (deselected) */

  stm32_gpio_config(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_MODER_OUTPUT,
                    GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, 0);
  stm32_gpio_write(SPI1_CS_PORT, SPI1_CS_PIN, true);
}
#endif

#ifdef CONFIG_STM32N6_SPI2
void stm32_spi2initialize(void)
{
  /* Configure CS pin as GPIO output, push-pull, high (deselected) */

  stm32_gpio_config(SPI2_CS_PORT, SPI2_CS_PIN, GPIO_MODER_OUTPUT,
                    GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, 0);
  stm32_gpio_write(SPI2_CS_PORT, SPI2_CS_PIN, true);
}
#endif

#ifdef CONFIG_STM32N6_SPI3
void stm32_spi3initialize(void)
{
  /* Configure CS pin as GPIO output, push-pull, high (deselected) */

  stm32_gpio_config(SPI3_CS_PORT, SPI3_CS_PIN, GPIO_MODER_OUTPUT,
                    GPIO_OTYPER_PP, GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, 0);
  stm32_gpio_write(SPI3_CS_PORT, SPI3_CS_PIN, true);
}
#endif
