/****************************************************************************
 * include/nuttx/misc/thermal_printer.h
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

#ifndef __INCLUDE_NUTTX_MISC_THERMAL_PRINTER_H
#define __INCLUDE_NUTTX_MISC_THERMAL_PRINTER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/spi/spi.h>
#include <nuttx/fs/ioctl.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Thermal Printer IOCTL commands ******************************************/

#define _TPIOC(nr)             _IOC(_TPIOCBASE, nr)

/* Command: TPIOC_SET_DENSITY
 * Description: Set the print heating density
 * Argument: unsigned int value (0-255, default ~120)
 */

#define TPIOC_SET_DENSITY      _TPIOC(0x0001)

/* Command: TPIOC_CUT
 * Description: Execute paper cut
 * Argument: unsigned int style (0=full cut, 1=partial cut)
 */

#define TPIOC_CUT              _TPIOC(0x0002)

/* Command: TPIOC_FEED
 * Description: Feed N lines of blank paper
 * Argument: unsigned int number of lines to feed
 */

#define TPIOC_FEED             _TPIOC(0x0003)

/* Command: TPIOC_INIT
 * Description: Re-initialize the printer to defaults
 * Argument: none (ignored)
 */

#define TPIOC_INIT             _TPIOC(0x0004)

/* ESC/POS Protocol Commands ***********************************************/

/* Init & Reset */

#define ESC_AT                 0x1b, 0x40       /* ESC @ - Initialize printer */

/* Print Control */

#define ESC_D_NUL              0x1b, 0x44, 0x00 /* ESC D NUL - Clear tabs */

/* Line Feed / Paper Feed */

#define LF                     0x0a             /* Line feed */
#define CR                     0x0d             /* Carriage return */
#define ESC_D                  0x1b, 0x64       /* ESC d n - Print and feed n lines */

/* Cut */

#define GS_V_N                 0x1d, 0x56, 0x00 /* GS V 0 - Full cut */
#define GS_V_PARTIAL           0x1d, 0x56, 0x01 /* GS V 1 - Partial cut */

/* Heating Control */

#define ESC_7                   0x1b, 0x37      /* ESC 7 n - Set heating params */
#define GS_8                    0x1d, 0x38      /* GS 8 - Set heating density */

/* Character Set */

#define ESC_t                   0x1b, 0x74      /* ESC t n - Select character set */

/* Default printer parameters */

#define THERMAL_PRINTER_DEFAULT_DENSITY  120
#define THERMAL_PRINTER_MAX_DENSITY      255
#define THERMAL_PRINTER_LINE_HEIGHT      24   /* pixels per line */

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: thermal_printer_register
 *
 * Description:
 *   Register the thermal printer character device (/dev/tp0).
 *
 * Input Parameters:
 *   spi - An instance of the SPI device used to communicate with the
 *         thermal printer.
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int thermal_printer_register(FAR struct spi_dev_s *spi);

#endif /* __INCLUDE_NUTTX_MISC_THERMAL_PRINTER_H */
