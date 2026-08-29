/****************************************************************************
 * drivers/ai/tinyml/img_classifier.c
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
#include <nuttx/ai/tinyml.h>
#include <nuttx/video/video.h>
#include <nuttx/video/fb.h>
#include <nuttx/fs/fs.h>
#include <nuttx/kmalloc.h>
#include <nuttx/log.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define IMG_CLASSIFIER_TAG    "img_cls"
#define CAMERA_DEVICE         "/dev/video0"
#define FB_DEVICE             "/dev/fb0"
#define PRINTER_DEVICE        "/dev/tp0"

/* Default image dimensions for classification */

#define DEFAULT_IMG_WIDTH     96
#define DEFAULT_IMG_HEIGHT    96
#define DEFAULT_IMG_CHANNELS  3

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct img_classifier_s
{
  struct tinyml_model_s model;
  struct tinyml_img_config_s config;
  int camera_fd;
  int fb_fd;
  int printer_fd;
  FAR uint8_t *frame_buffer;
  FAR uint8_t *resized_buffer;
  bool initialized;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct img_classifier_s g_classifier;

/* Example labels for demo */

static const char *g_demo_labels[] =
{
  "cat", "dog", "bird", "car", "plane",
  "ship", "truck", "horse", "deer", "frog"
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int init_camera(FAR struct img_classifier_s *cls)
{
  cls->camera_fd = open(CAMERA_DEVICE, O_RDONLY);
  if (cls->camera_fd < 0)
    {
      ai_err("Failed to open camera: %d\n", errno);
      return -errno;
    }

  /* Configure camera for image capture */

  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = cls->config.width;
  fmt.fmt.pix.height = cls->config.height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

  int ret = ioctl(cls->camera_fd, VIDIOC_S_FMT, &fmt);
  if (ret < 0)
    {
      ai_err("Failed to set camera format: %d\n", errno);
      close(cls->camera_fd);
      return -errno;
    }

  return OK;
}

static int init_framebuffer(FAR struct img_classifier_s *cls)
{
  cls->fb_fd = open(FB_DEVICE, O_WRONLY);
  if (cls->fb_fd < 0)
    {
      ai_err("Failed to open framebuffer: %d\n", errno);
      return -errno;
    }

  return OK;
}

static int init_printer(FAR struct img_classifier_s *cls)
{
  cls->printer_fd = open(PRINTER_DEVICE, O_WRONLY);
  if (cls->printer_fd < 0)
    {
      ai_warn("Printer not available: %d\n", errno);
      cls->printer_fd = -1;
    }

  return OK;
}

static int capture_frame(FAR struct img_classifier_s *cls)
{
  if (cls->camera_fd < 0)
    {
      return -EINVAL;
    }

  /* Read frame from camera */

  ssize_t bytes_read = read(cls->camera_fd, cls->frame_buffer,
                            cls->config.width * cls->config.height *
                            cls->config.channels);
  if (bytes_read < 0)
    {
      ai_err("Failed to read frame: %d\n", errno);
      return -errno;
    }

  return OK;
}

static void resize_image_bilinear(FAR const uint8_t *src,
                                  uint32_t src_w, uint32_t src_h,
                                  FAR uint8_t *dst,
                                  uint32_t dst_w, uint32_t dst_h,
                                  uint32_t channels)
{
  float x_ratio = (float)src_w / dst_w;
  float y_ratio = (float)src_h / dst_h;

  for (uint32_t y = 0; y < dst_h; y++)
    {
      for (uint32_t x = 0; x < dst_w; x++)
        {
          float src_x = x * x_ratio;
          float src_y = y * y_ratio;
          int x_low = (int)src_x;
          int y_low = (int)src_y;
          int x_high = x_low + 1 < src_w ? x_low + 1 : x_low;
          int y_high = y_low + 1 < src_h ? y_low + 1 : y_low;

          float x_weight = src_x - x_low;
          float y_weight = src_y - y_low;

          for (uint32_t c = 0; c < channels; c++)
            {
              float val =
                  src[(y_low * src_w + x_low) * channels + c] *
                  (1 - x_weight) * (1 - y_weight) +
                  src[(y_low * src_w + x_high) * channels + c] *
                  x_weight * (1 - y_weight) +
                  src[(y_high * src_w + x_low) * channels + c] *
                  (1 - x_weight) * y_weight +
                  src[(y_high * src_w + x_high) * channels + c] *
                  x_weight * y_weight;

              dst[(y * dst_w + x) * channels + c] = (uint8_t)val;
            }
        }
    }
}

static int display_result(FAR struct img_classifier_s *cls,
                          FAR struct tinyml_result_s *result)
{
  if (cls->fb_fd < 0)
    {
      return OK;
    }

  /* Simple text overlay - in real implementation, use LVGL or framebuffer
   * drawing
   */

  char text[64];
  snprintf(text, sizeof(text), "Class: %s (%.1f%%)",
           cls->config.labels[result->class_id],
           result->confidence * 100.0f);

  ai_info("Classification result: %s\n", text);

  /* TODO: Draw text on framebuffer */

  return OK;
}

static int print_result(FAR struct img_classifier_s *cls,
                        FAR struct tinyml_result_s *result)
{
  if (cls->printer_fd < 0)
    {
      return OK;
    }

  /* Print classification result using ESC/POS */

  char receipt[256];
  int len = snprintf(receipt, sizeof(receipt),
                     "\x1b\x40"         /* ESC @ - Initialize */
                     "\x1b\x61\x01"     /* ESC a 1 - Center align */
                     "=== AI Classification ===\r\n\r\n"
                     "\x1b\x61\x00"     /* ESC a 0 - Left align */
                     "Result: %s\r\n"
                     "Confidence: %.1f%%\r\n\r\n"
                     "\x1d\x56\x00"     /* GS V 0 - Full cut */
                     "\r\n\r\n",
                     cls->config.labels[result->class_id],
                     result->confidence * 100.0f);

  ssize_t written = write(cls->printer_fd, receipt, len);
  if (written < 0)
    {
      ai_err("Failed to print: %d\n", errno);
      return -errno;
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int img_classifier_init(FAR struct tinyml_model_s *model,
                        FAR const char **labels, uint32_t num_labels)
{
  FAR struct img_classifier_s *cls = &g_classifier;

  memset(cls, 0, sizeof(struct img_classifier_s));

  /* Copy model */

  memcpy(&cls->model, model, sizeof(struct tinyml_model_s));

  /* Configure image parameters */

  cls->config.width = DEFAULT_IMG_WIDTH;
  cls->config.height = DEFAULT_IMG_HEIGHT;
  cls->config.channels = DEFAULT_IMG_CHANNELS;
  cls->config.input_type = TINYML_TYPE_INT8;
  cls->config.scale = 0.003922f;  /* 1/255 */
  cls->config.zero_point = 0;
  cls->config.labels = labels ? labels : g_demo_labels;
  cls->config.num_labels = num_labels ? num_labels : 10;

  /* Allocate buffers */

  cls->frame_buffer = kmm_malloc(cls->config.width * cls->config.height *
                                 cls->config.channels * 2);
  if (cls->frame_buffer == NULL)
    {
      return -ENOMEM;
    }

  cls->resized_buffer = cls->frame_buffer +
      cls->config.width * cls->config.height * cls->config.channels;

  /* Initialize peripherals */

  int ret = init_camera(cls);
  if (ret < 0)
    {
      ai_warn("Camera init failed, continuing without camera\n");
    }

  ret = init_framebuffer(cls);
  if (ret < 0)
    {
      ai_warn("Framebuffer init failed, continuing without display\n");
    }

  ret = init_printer(cls);
  if (ret < 0)
    {
      ai_warn("Printer init failed, continuing without printer\n");
    }

  cls->initialized = true;
  ai_info("Image classifier initialized\n");

  return OK;
}

int img_classifier_run(FAR struct tinyml_result_s *result)
{
  FAR struct img_classifier_s *cls = &g_classifier;

  if (!cls->initialized)
    {
      return -EINVAL;
    }

  /* Capture image from camera */

  int ret = capture_frame(cls);
  if (ret < 0)
    {
      return ret;
    }

  /* Resize image to model input size */

  resize_image_bilinear(cls->frame_buffer,
                        cls->config.width * 2,
                        cls->config.height * 2,
                        cls->resized_buffer,
                        cls->config.width,
                        cls->config.height,
                        cls->config.channels);

  /* Run classification */

  ret = tinyml_img_classify(&cls->model, cls->resized_buffer,
                            &cls->config, result);
  if (ret < 0)
    {
      ai_err("Classification failed: %d\n", ret);
      return ret;
    }

  /* Display result on LCD */

  display_result(cls, result);

  /* Print result */

  print_result(cls, result);

  return OK;
}

void img_classifier_deinit(void)
{
  FAR struct img_classifier_s *cls = &g_classifier;

  if (cls->camera_fd >= 0)
    {
      close(cls->camera_fd);
    }

  if (cls->fb_fd >= 0)
    {
      close(cls->fb_fd);
    }

  if (cls->printer_fd >= 0)
    {
      close(cls->printer_fd);
    }

  if (cls->frame_buffer != NULL)
    {
      kmm_free(cls->frame_buffer);
    }

  tinyml_free_model(&cls->model);

  cls->initialized = false;
  ai_info("Image classifier deinitialized\n");
}
