/****************************************************************************
 * arch/arm/src/stm32n6/stm32_dcmipp_imgdata.c
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
#include <nuttx/video/imgdata.h>
#include <nuttx/video/imgsensor.h>
#include <nuttx/kmalloc.h>
#include <debug.h>
#include <errno.h>

#include "stm32_dcmipp.h"
#include "hardware/stm32_dcmipp.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Default resolution */

#define DCMIPP_IMGDATA_DEFAULT_WIDTH   640
#define DCMIPP_IMGDATA_DEFAULT_HEIGHT  480

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stm32_dcmipp_imgdata_s
{
  struct imgdata_s  dev;           /* Must be first */
  uint8_t          *buf_addr;
  uint32_t          buf_size;
  uint16_t          width;
  uint16_t          height;
  uint32_t          pixelformat;
  imgdata_capture_t callback;
  void             *callback_arg;
  bool              capturing;
  bool              interface_csi; /* true = CSI, false = parallel */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int dcmipp_imgdata_init(FAR struct imgdata_s *data);
static int dcmipp_imgdata_uninit(FAR struct imgdata_s *data);
static int dcmipp_imgdata_set_buf(FAR struct imgdata_s *data,
                                   uint8_t nr_datafmts,
                                   FAR imgdata_format_t *datafmts,
                                   uint8_t *addr, uint32_t size);
static int dcmipp_imgdata_validate(FAR struct imgdata_s *data,
                                    uint8_t nr_datafmts,
                                    FAR imgdata_format_t *datafmts,
                                    FAR imgdata_interval_t *interval);
static int dcmipp_imgdata_start_capture(FAR struct imgdata_s *data,
                                         uint8_t nr_datafmts,
                                         FAR imgdata_format_t *datafmts,
                                         FAR imgdata_interval_t *interval,
                                         FAR imgdata_capture_t callback,
                                         FAR void *arg);
static int dcmipp_imgdata_stop_capture(FAR struct imgdata_s *data);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct imgdata_ops_s g_dcmipp_imgdata_ops =
{
  .init                  = dcmipp_imgdata_init,
  .uninit                = dcmipp_imgdata_uninit,
  .set_buf               = dcmipp_imgdata_set_buf,
  .validate_frame_setting = dcmipp_imgdata_validate,
  .start_capture         = dcmipp_imgdata_start_capture,
  .stop_capture          = dcmipp_imgdata_stop_capture,
  .alloc                 = NULL,
  .free                  = NULL,
};

static struct stm32_dcmipp_imgdata_s g_dcmipp_imgdata;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: dcmipp_frame_done
 *
 * Description:
 *   Frame completion callback from DCMIPP ISR.
 *
 ****************************************************************************/

static void dcmipp_frame_done(uint32_t pipe, void *arg)
{
  struct stm32_dcmipp_imgdata_s *priv = (struct stm32_dcmipp_imgdata_s *)arg;
  struct timeval ts;

  UNUSED(pipe);

  if (priv != NULL && priv->callback != NULL && priv->capturing)
    {
      /* Get timestamp */

      gettimeofday(&ts, NULL);

      /* Notify upper layer of frame completion */

      priv->callback(0, priv->buf_size, &ts, priv->callback_arg);
    }
}

/****************************************************************************
 * Name: dcmipp_imgdata_init
 ****************************************************************************/

static int dcmipp_imgdata_init(FAR struct imgdata_s *data)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;
  int ret;

  /* Initialize the DCMIPP hardware */

  ret = stm32_dcmipp_init();
  if (ret < 0)
    {
      verr("ERROR: Failed to initialize DCMIPP: %d\n", ret);
      return ret;
    }

  /* Register the frame callback for pipe 1 (main pipe) */

  stm32_dcmipp_register_callback(DCMIPP_PIPE1, dcmipp_frame_done, priv);

  priv->capturing = false;

  return 0;
}

/****************************************************************************
 * Name: dcmipp_imgdata_uninit
 ****************************************************************************/

static int dcmipp_imgdata_uninit(FAR struct imgdata_s *data)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;

  if (priv->capturing)
    {
      stm32_dcmipp_pipe_stop(DCMIPP_PIPE1);
      priv->capturing = false;
    }

  stm32_dcmipp_deinit();

  return 0;
}

/****************************************************************************
 * Name: dcmipp_imgdata_set_buf
 ****************************************************************************/

static int dcmipp_imgdata_set_buf(FAR struct imgdata_s *data,
                                   uint8_t nr_datafmts,
                                   FAR imgdata_format_t *datafmts,
                                   uint8_t *addr, uint32_t size)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;

  UNUSED(nr_datafmts);
  UNUSED(datafmts);

  priv->buf_addr = addr;
  priv->buf_size = size;

  /* Set the DMA destination address for pipe 1 */

  return stm32_dcmipp_pipe_setbuf(DCMIPP_PIPE1, (uint32_t)addr);
}

/****************************************************************************
 * Name: dcmipp_imgdata_validate
 ****************************************************************************/

static int dcmipp_imgdata_validate(FAR struct imgdata_s *data,
                                    uint8_t nr_datafmts,
                                    FAR imgdata_format_t *datafmts,
                                    FAR imgdata_interval_t *interval)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;

  UNUSED(nr_datafmts);
  UNUSED(interval);

  if (datafmts == NULL)
    {
      return -EINVAL;
    }

  /* Accept any resolution up to 2592x1944 (max for IMX335) */

  if (datafmts[0].width == 0 || datafmts[0].width > 2592 ||
      datafmts[0].height == 0 || datafmts[0].height > 1944)
    {
      verr("ERROR: Invalid resolution %dx%d\n",
             datafmts[0].width, datafmts[0].height);
      return -EINVAL;
    }

  /* Check pixel format */

  switch (datafmts[0].pixelformat)
    {
      case IMGDATA_PIX_FMT_RGB565:
      case IMGDATA_PIX_FMT_RGB565X:
      case IMGDATA_PIX_FMT_UYVY:
      case IMGDATA_PIX_FMT_YUYV:
      case IMGDATA_PIX_FMT_NV12:
      case IMGDATA_PIX_FMT_YUV420P:
        break;

      default:
        verr("ERROR: Unsupported pixel format: %d\n",
               datafmts[0].pixelformat);
        return -EINVAL;
    }

  priv->width = datafmts[0].width;
  priv->height = datafmts[0].height;
  priv->pixelformat = datafmts[0].pixelformat;

  return 0;
}

/****************************************************************************
 * Name: dcmipp_imgdata_start_capture
 ****************************************************************************/

static int dcmipp_imgdata_start_capture(FAR struct imgdata_s *data,
                                         uint8_t nr_datafmts,
                                         FAR imgdata_format_t *datafmts,
                                         FAR imgdata_interval_t *interval,
                                         FAR imgdata_capture_t callback,
                                         FAR void *arg)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;
  uint32_t pp_format;
  int ret;

  UNUSED(nr_datafmts);
  UNUSED(interval);

  if (datafmts == NULL)
    {
      return -EINVAL;
    }

  /* Store callback info */

  priv->callback = callback;
  priv->callback_arg = arg;
  priv->width = datafmts[0].width;
  priv->height = datafmts[0].height;
  priv->pixelformat = datafmts[0].pixelformat;

  /* Map pixel format to DCMIPP pixel packer format */

  switch (datafmts[0].pixelformat)
    {
      case IMGDATA_PIX_FMT_RGB565:
      case IMGDATA_PIX_FMT_RGB565X:
        pp_format = DCMIPP_PIXEL_PACKER_RGB565_1;
        break;

      case IMGDATA_PIX_FMT_UYVY:
      case IMGDATA_PIX_FMT_YUYV:
        pp_format = DCMIPP_PIXEL_PACKER_YUV422_1;
        break;

      case IMGDATA_PIX_FMT_NV12:
      case IMGDATA_PIX_FMT_YUV420P:
        pp_format = DCMIPP_PIXEL_PACKER_YUV420_2;
        break;

      default:
        pp_format = DCMIPP_PIXEL_PACKER_RGB565_1;
        break;
    }

  /* Configure the pipe
   * For parallel (OV5640): use YUV422_8 data type
   * For CSI (IMX335): use RAW10 data type
   */

  if (priv->interface_csi)
    {
      ret = stm32_dcmipp_pipe_config(DCMIPP_PIPE1,
                                      CSI_DT_RAW10, 0,
                                      pp_format,
                                      DCMIPP_FRAME_RATE_ALL);
    }
  else
    {
      ret = stm32_dcmipp_pipe_config(DCMIPP_PIPE1,
                                      CSI_DT_YUV422_8, 0,
                                      pp_format,
                                      DCMIPP_FRAME_RATE_ALL);
    }

  if (ret < 0)
    {
      verr("ERROR: Failed to configure pipe: %d\n", ret);
      return ret;
    }

  /* Set destination buffer */

  if (priv->buf_addr != NULL)
    {
      ret = stm32_dcmipp_pipe_setbuf(DCMIPP_PIPE1,
                                      (uint32_t)priv->buf_addr);
      if (ret < 0)
        {
          verr("ERROR: Failed to set buffer: %d\n", ret);
          return ret;
        }
    }

  /* Start continuous capture */

  priv->capturing = true;

  ret = stm32_dcmipp_pipe_start(DCMIPP_PIPE1, DCMIPP_MODE_CONTINUOUS);
  if (ret < 0)
    {
      verr("ERROR: Failed to start capture: %d\n", ret);
      priv->capturing = false;
      return ret;
    }

  return 0;
}

/****************************************************************************
 * Name: dcmipp_imgdata_stop_capture
 ****************************************************************************/

static int dcmipp_imgdata_stop_capture(FAR struct imgdata_s *data)
{
  struct stm32_dcmipp_imgdata_s *priv =
    (struct stm32_dcmipp_imgdata_s *)data;
  int ret;

  if (!priv->capturing)
    {
      return 0;
    }

  ret = stm32_dcmipp_pipe_stop(DCMIPP_PIPE1);
  priv->capturing = false;

  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_dcmipp_imgdata_register
 *
 * Description:
 *   Register the DCMIPP image data device.
 *
 * Input Parameters:
 *   interface_csi - true for CSI interface (IMX335), false for parallel
 *                   (OV5640)
 *
 * Returned Value:
 *   Pointer to the imgdata_s structure, or NULL on failure.
 *
 ****************************************************************************/

FAR struct imgdata_s *stm32_dcmipp_imgdata_register(bool interface_csi)
{
  struct stm32_dcmipp_imgdata_s *priv = &g_dcmipp_imgdata;

  memset(priv, 0, sizeof(*priv));

  priv->dev.ops = &g_dcmipp_imgdata_ops;
  priv->interface_csi = interface_csi;

  /* Register with the V4L2 framework */

  imgdata_register(&priv->dev);

  vinfo("DCMIPP imgdata registered (interface=%s)\n",
          interface_csi ? "CSI" : "parallel");

  return &priv->dev;
}
