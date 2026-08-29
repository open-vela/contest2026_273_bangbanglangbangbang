# arch/arm/src/stm32n6/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Architecture-specific sources

set(SRCS
  stm32_start.c
  stm32_clockconfig.c
  stm32_gpio.c
  stm32_uart.c
  stm32_irq.c
  stm32_timer.c
  stm32_serial.c
  stm32_lowputc.c
)

if(CONFIG_STM32N6_I2C1 OR CONFIG_STM32N6_I2C2 OR CONFIG_STM32N6_I2C3 OR CONFIG_STM32N6_I2C4)
  list(APPEND SRCS stm32_i2c.c)
endif()

if(CONFIG_STM32N6_SPI1 OR CONFIG_STM32N6_SPI2 OR CONFIG_STM32N6_SPI3)
  list(APPEND SRCS stm32_spi.c)
endif()

if(CONFIG_STM32N6_XSPI2)
  list(APPEND SRCS stm32_xspi.c)
endif()

target_sources(arch PRIVATE ${SRCS})
