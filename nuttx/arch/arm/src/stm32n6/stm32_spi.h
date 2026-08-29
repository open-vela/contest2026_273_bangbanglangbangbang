/****************************************************************************
 * arch/arm/src/stm32n6/stm32_spi.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
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

#ifndef __ARCH_ARM_SRC_STM32N6_STM32_SPI_H
#define __ARCH_ARM_SRC_STM32N6_STM32_SPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: stm32_spibus_initialize
 *
 * Description:
 *   Initialize the selected SPI bus
 *
 * Input Parameters:
 *   Port number (for hardware that has multiple SPI interfaces)
 *
 * Returned Value:
 *   Valid SPI device structure reference on success; a NULL on failure
 *
 ****************************************************************************/

struct spi_dev_s;
FAR struct spi_dev_s *stm32_spibus_initialize(int bus);

/****************************************************************************
 * Name:  stm32_spi1/2/3select and stm32_spi1/2/3status
 *
 * Description:
 *   The external functions, stm32_spi1/2/3select and stm32_spi1/2/3status
 *   must be provided by board-specific logic.  They are implementations of
 *   the select and status methods of the SPI interface defined by struct
 *   spi_ops_s (see include/nuttx/spi/spi.h).
 *
 ****************************************************************************/

/* SPI1 */

#ifdef CONFIG_STM32N6_SPI1
void stm32_spi1select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected);
uint8_t stm32_spi1status(FAR struct spi_dev_s *dev, uint32_t devid);
#  ifdef CONFIG_SPI_CMDDATA
int stm32_spi1cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd);
#  endif
#  ifdef CONFIG_SPI_CALLBACK
int stm32_spi1register(FAR struct spi_dev_s *dev,
                       spi_mediachange_t callback, FAR void *arg);
#  endif
#endif

/* SPI2 */

#ifdef CONFIG_STM32N6_SPI2
void stm32_spi2select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected);
uint8_t stm32_spi2status(FAR struct spi_dev_s *dev, uint32_t devid);
#  ifdef CONFIG_SPI_CMDDATA
int stm32_spi2cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd);
#  endif
#  ifdef CONFIG_SPI_CALLBACK
int stm32_spi2register(FAR struct spi_dev_s *dev,
                       spi_mediachange_t callback, FAR void *arg);
#  endif
#endif

/* SPI3 */

#ifdef CONFIG_STM32N6_SPI3
void stm32_spi3select(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool selected);
uint8_t stm32_spi3status(FAR struct spi_dev_s *dev, uint32_t devid);
#  ifdef CONFIG_SPI_CMDDATA
int stm32_spi3cmddata(FAR struct spi_dev_s *dev, uint32_t devid,
                      bool cmd);
#  endif
#  ifdef CONFIG_SPI_CALLBACK
int stm32_spi3register(FAR struct spi_dev_s *dev,
                       spi_mediachange_t callback, FAR void *arg);
#  endif
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_STM32N6_STM32_SPI_H */
