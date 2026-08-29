/****************************************************************************
 * arch/arm/src/stm32n6/stm32_xspi.h
 *
 * SPDX-License-Identifier: Apache-2.0
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

#ifndef __ARCH_ARM_SRC_STM32N6_STM32_XSPI_H
#define __ARCH_ARM_SRC_STM32N6_STM32_XSPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/spi/qspi.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef CONFIG_STM32N6_XSPI2

/****************************************************************************
 * Name: stm32n6_xspi_initialize
 *
 * Description:
 *   Initialize the XSPI2 peripheral and return a QSPI device instance.
 *
 * Input Parameters:
 *   intf - Interface number (must be 2 for XSPI2)
 *
 * Returned Value:
 *   Valid QSPI device structure reference on success; NULL on failure.
 *
 ****************************************************************************/

FAR struct qspi_dev_s *stm32n6_xspi_initialize(int intf);

/****************************************************************************
 * Name: stm32n6_xspi_enter_memorymapped
 *
 * Description:
 *   Put the XSPI device into memory-mapped mode for direct read/write
 *   access to the external NOR Flash.
 *
 ****************************************************************************/

void stm32n6_xspi_enter_memorymapped(void);

/****************************************************************************
 * Name: stm32n6_xspi_exit_memorymapped
 *
 * Description:
 *   Take the XSPI device out of memory-mapped mode.
 *
 ****************************************************************************/

void stm32n6_xspi_exit_memorymapped(void);

#endif /* CONFIG_STM32N6_XSPI2 */
#endif /* __ARCH_ARM_SRC_STM32N6_STM32_XSPI_H */
