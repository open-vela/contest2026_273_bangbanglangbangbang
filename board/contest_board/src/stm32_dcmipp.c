/****************************************************************************
 * boards/arm/stm32n6/stm32n647-evb/src/stm32_dcmipp.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/arch.h>
#include <debug.h>

#include <nuttx/video/imx335.h>
#include <nuttx/video/ov5640.h>

#include "stm32_gpio.h"
#include "hardware/stm32_gpio.h"
#include "stm32_i2c.h"
#include "stm32_dcmipp.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* IMX335 GPIO pins (direct port/pin addressing) */

#define IMX335_PWDN_PORT  GPIO_PORTG
#define IMX335_PWDN_PIN   6
#define IMX335_RST_PORT   GPIO_PORTG
#define IMX335_RST_PIN    4

/* OV5640 GPIO pins (direct port/pin addressing) */

#define OV5640_PWDN_PORT  GPIO_PORTG
#define OV5640_PWDN_PIN   14
#define OV5640_RST_PORT   GPIO_PORTG
#define OV5640_RST_PIN    2

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_gpio_output_write
 *
 * Description:
 *   Configure a GPIO pin as output and set its initial value.
 *
 ****************************************************************************/

static void board_gpio_output_write(uint8_t port, uint8_t pin, bool value)
{
  stm32_gpio_config(port, pin, GPIO_MODER_OUTPUT, GPIO_OTYPER_PP,
                    GPIO_OSPEEDR_HIGH, GPIO_PUPDR_NONE, 0);
  stm32_gpio_write(port, pin, value);
}

/****************************************************************************
 * Name: board_dcmipp_imx335_init
 *
 * Description:
 *   Initialize IMX335 sensor: power sequence, reset, register with V4L2.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_DCMIPP_IMX335
static int board_dcmipp_imx335_init(void)
{
  FAR struct i2c_master_s *i2c;
  bool csi_interface = true;
  int ret;

  vinfo("Initializing IMX335 camera sensor\n");

  /* Configure PWDN pin (PG6) - active high to power down */

  board_gpio_output_write(IMX335_PWDN_PORT, IMX335_PWDN_PIN, false);

  /* Configure RST pin (PG4) - active low reset */

  board_gpio_output_write(IMX335_RST_PORT, IMX335_RST_PIN, true);

  /* Wait for power to stabilize */

  up_mdelay(10);

  /* Assert reset */

  stm32_gpio_write(IMX335_RST_PORT, IMX335_RST_PIN, false);
  up_mdelay(10);

  /* Release reset */

  stm32_gpio_write(IMX335_RST_PORT, IMX335_RST_PIN, true);
  up_mdelay(20);

  /* Get I2C bus */

#ifdef CONFIG_STM32N6_I2C1
  i2c = stm32_i2cbus_initialize(1);
#elif defined(CONFIG_STM32N6_I2C2)
  i2c = stm32_i2cbus_initialize(2);
#elif defined(CONFIG_STM32N6_I2C3)
  i2c = stm32_i2cbus_initialize(3);
#else
  verr("ERROR: No I2C bus configured for IMX335\n");
  return -ENODEV;
#endif

  if (i2c == NULL)
    {
      verr("ERROR: Failed to get I2C bus\n");
      return -ENODEV;
    }

  /* Register the DCMIPP imgdata device (CSI mode) */

  stm32_dcmipp_imgdata_register(csi_interface);

  /* Register the IMX335 sensor */

  ret = imx335_register(i2c);
  if (ret < 0)
    {
      verr("ERROR: Failed to register IMX335: %d\n", ret);
      return ret;
    }

  return 0;
}
#endif /* CONFIG_STM32N6_DCMIPP_IMX335 */

/****************************************************************************
 * Name: board_dcmipp_ov5640_init
 *
 * Description:
 *   Initialize OV5640 sensor: power sequence, reset, register with V4L2.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32N6_DCMIPP_OV5640
static int board_dcmipp_ov5640_init(void)
{
  FAR struct i2c_master_s *i2c;
  bool csi_interface = false;
  int ret;

  vinfo("Initializing OV5640 camera sensor\n");

  /* Configure PWDN pin (PG14) - active high to power down */

  board_gpio_output_write(OV5640_PWDN_PORT, OV5640_PWDN_PIN, false);

  /* Configure RST pin (PQ2) - active low reset */

  board_gpio_output_write(OV5640_RST_PORT, OV5640_RST_PIN, true);

  /* Power on sequence */

  up_mdelay(10);

  /* Assert reset */

  stm32_gpio_write(OV5640_RST_PORT, OV5640_RST_PIN, false);
  up_mdelay(20);

  /* Release reset */

  stm32_gpio_write(OV5640_RST_PORT, OV5640_RST_PIN, true);
  up_mdelay(20);

  /* Get I2C bus (OV5640 typically uses I2C2 on STM32N647-EVB) */

#ifdef CONFIG_STM32N6_I2C2
  i2c = stm32_i2cbus_initialize(2);
#elif defined(CONFIG_STM32N6_I2C1)
  i2c = stm32_i2cbus_initialize(1);
#elif defined(CONFIG_STM32N6_I2C3)
  i2c = stm32_i2cbus_initialize(3);
#else
  verr("ERROR: No I2C bus configured for OV5640\n");
  return -ENODEV;
#endif

  if (i2c == NULL)
    {
      verr("ERROR: Failed to get I2C bus\n");
      return -ENODEV;
    }

  /* Register the DCMIPP imgdata device (parallel mode) */

  stm32_dcmipp_imgdata_register(csi_interface);

  /* Register the OV5640 sensor */

  ret = ov5640_register(i2c);
  if (ret < 0)
    {
      verr("ERROR: Failed to register OV5640: %d\n", ret);
      return ret;
    }

  return 0;
}
#endif /* CONFIG_STM32N6_DCMIPP_OV5640 */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_dcmipp_initialize
 *
 * Description:
 *   Initialize DCMIPP camera subsystem. Called from board_initialize().
 *
 ****************************************************************************/

int board_dcmipp_initialize(void)
{
  int ret = 0;

#ifdef CONFIG_STM32N6_DCMIPP
  vinfo("Initializing DCMIPP camera subsystem\n");

#ifdef CONFIG_STM32N6_DCMIPP_OV5640
  ret = board_dcmipp_ov5640_init();
  if (ret < 0)
    {
      verr("ERROR: OV5640 init failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_STM32N6_DCMIPP_IMX335
  ret = board_dcmipp_imx335_init();
  if (ret < 0)
    {
      verr("ERROR: IMX335 init failed: %d\n", ret);
    }
#endif

#endif /* CONFIG_STM32N6_DCMIPP */

  return ret;
}
