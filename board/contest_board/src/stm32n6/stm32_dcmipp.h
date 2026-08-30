/****************************************************************************
 * arch/arm/src/stm32n6/stm32_dcmipp.h
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

#ifndef __ARCH_ARM_SRC_STM32N6_STM32_DCMIPP_H
#define __ARCH_ARM_SRC_STM32N6_STM32_DCMIPP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/video/imgdata.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Pipe identifiers */

#define DCMIPP_PIPE0  0   /* Dump pipe */
#define DCMIPP_PIPE1  1   /* Main pipe */
#define DCMIPP_PIPE2  2   /* Ancillary pipe */

/* Interface mode */

#define DCMIPP_MODE_PARALLEL  0
#define DCMIPP_MODE_CSI       1

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Parallel interface configuration */

struct dcmipp_parallel_cfg_s
{
  uint32_t format;            /* DCMIPP_FORMAT_xxx */
  uint32_t extended_data;     /* DCMIPP_INTERFACE_xxBITS */
  uint32_t vsync_pol;         /* VSYNC polarity */
  uint32_t hsync_pol;         /* HSYNC polarity */
  uint32_t pck_pol;           /* Pixel clock polarity */
};

/* CSI interface configuration */

struct dcmipp_csi_cfg_s
{
  uint32_t num_lanes;         /* Number of data lanes (1 or 2) */
  uint32_t lane_mapping;      /* Lane mapping (physical or inverted) */
  uint32_t phy_bitrate;       /* PHY bitrate (CSI_PHY_BT_xxx) */
};

/* Frame callback */

typedef void (*dcmipp_frame_callback_t)(uint32_t pipe, void *arg);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Name: stm32_dcmipp_init
 *
 * Description:
 *   Initialize the DCMIPP peripheral: enable clocks, reset, configure
 *   IPPLUG defaults.
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_init(void);

/****************************************************************************
 * Name: stm32_dcmipp_deinit
 *
 * Description:
 *   Deinitialize the DCMIPP peripheral.
 *
 ****************************************************************************/

void stm32_dcmipp_deinit(void);

/****************************************************************************
 * Name: stm32_dcmipp_parallel_config
 *
 * Description:
 *   Configure the DCMIPP parallel interface for camera capture.
 *
 * Input Parameters:
 *   cfg - Parallel interface configuration
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_parallel_config(const struct dcmipp_parallel_cfg_s *cfg);

/****************************************************************************
 * Name: stm32_dcmipp_csi_config
 *
 * Description:
 *   Configure the DCMIPP CSI interface for camera capture.
 *
 * Input Parameters:
 *   cfg - CSI interface configuration
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_csi_config(const struct dcmipp_csi_cfg_s *cfg);

/****************************************************************************
 * Name: stm32_dcmipp_pipe_config
 *
 * Description:
 *   Configure a DCMIPP pipe with the given data type, virtual channel,
 *   pixel packer format, and frame rate.
 *
 * Input Parameters:
 *   pipe       - Pipe number (0, 1, or 2)
 *   dt         - CSI data type (e.g. CSI_DT_RAW10)
 *   vc         - Virtual channel (0-3)
 *   pp_format  - Pixel packer output format
 *   framerate  - Frame rate divisor (DCMIPP_FRAME_RATE_xxx)
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_pipe_config(uint32_t pipe, uint32_t dt, uint32_t vc,
                              uint32_t pp_format, uint32_t framerate);

/****************************************************************************
 * Name: stm32_dcmipp_pipe_setbuf
 *
 * Description:
 *   Set the destination buffer address for a pipe.
 *
 * Input Parameters:
 *   pipe  - Pipe number (0, 1, or 2)
 *   addr  - Destination buffer address (must be 16-byte aligned)
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_pipe_setbuf(uint32_t pipe, uint32_t addr);

/****************************************************************************
 * Name: stm32_dcmipp_pipe_start
 *
 * Description:
 *   Start capture on the specified pipe.
 *
 * Input Parameters:
 *   pipe   - Pipe number (0, 1, or 2)
 *   mode   - Capture mode (DCMIPP_MODE_CONTINUOUS or DCMIPP_MODE_SNAPSHOT)
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_pipe_start(uint32_t pipe, uint32_t mode);

/****************************************************************************
 * Name: stm32_dcmipp_pipe_stop
 *
 * Description:
 *   Stop capture on the specified pipe.
 *
 * Input Parameters:
 *   pipe - Pipe number (0, 1, or 2)
 *
 * Returned Value:
 *   0 on success, negative errno on failure.
 *
 ****************************************************************************/

int stm32_dcmipp_pipe_stop(uint32_t pipe);

/****************************************************************************
 * Name: stm32_dcmipp_register_callback
 *
 * Description:
 *   Register a frame completion callback.
 *
 * Input Parameters:
 *   pipe     - Pipe number
 *   callback - Callback function (called from ISR context)
 *   arg      - User argument passed to callback
 *
 ****************************************************************************/

void stm32_dcmipp_register_callback(uint32_t pipe,
                                     dcmipp_frame_callback_t callback,
                                     void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __ARCH_ARM_SRC_STM32N6_STM32_DCMIPP_H */
