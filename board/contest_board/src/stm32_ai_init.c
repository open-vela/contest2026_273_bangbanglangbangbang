/****************************************************************************
 * boards/arm/stm32n6/stm32n647-evb/src/stm32_ai_init.c
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
#include <nuttx/ai/ai_agent_bridge.h>
#include <nuttx/ai/img_classifier.h>
#include <nuttx/log.h>
#include <syslog.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define AI_INIT_TAG  "ai_init"

/* Demo model data - in real implementation, load from XSPI Flash */

static const uint8_t g_demo_model_data[] =
{
  /* Model header */
  0x06,                                     /* Name length */
  'm', 'o', 'b', 'i', 'l', 'e',           /* Name: "mobile" */
  0x01,                                     /* Num layers */
  0x03,                                     /* Num tensors */

  /* Tensor 0: input */
  0x05,                                     /* Name length */
  'i', 'n', 'p', 'u', 't',                 /* Name: "input" */
  0x00,                                     /* Type: INT8 */
  0x04,                                     /* Num dims */
  0x00, 0x00, 0x00, 0x01,                   /* Dim 0: 1 */
  0x00, 0x00, 0x00, 0x60,                   /* Dim 1: 96 */
  0x00, 0x00, 0x00, 0x60,                   /* Dim 2: 96 */
  0x00, 0x00, 0x00, 0x03,                   /* Dim 3: 3 */

  /* Tensor 1: weights */
  0x06,                                     /* Name length */
  'w', 'e', 'i', 'g', 'h', 't',           /* Name: "weight" */
  0x00,                                     /* Type: INT8 */
  0x04,                                     /* Num dims */
  0x00, 0x00, 0x00, 0x0a,                   /* Dim 0: 10 */
  0x00, 0x00, 0x00, 0x03,                   /* Dim 1: 3 */
  0x00, 0x00, 0x00, 0x03,                   /* Dim 2: 3 */
  0x00, 0x00, 0x00, 0x03,                   /* Dim 3: 3 */

  /* Tensor 2: output */
  0x06,                                     /* Name length */
  'o', 'u', 't', 'p', 'u', 't',           /* Name: "output" */
  0x00,                                     /* Type: INT8 */
  0x02,                                     /* Num dims */
  0x00, 0x00, 0x00, 0x01,                   /* Dim 0: 1 */
  0x00, 0x00, 0x00, 0x0a,                   /* Dim 1: 10 */

  /* Layer 0: Conv2D */
  0x00,                                     /* Type: CONV2D */
  0x05,                                     /* Name length */
  'c', 'o', 'n', 'v', '1',                 /* Name: "conv1" */
  0x01,                                     /* Num inputs */
  0x01                                      /* Num outputs */
};

/* Demo labels */

static const char *g_demo_labels[] =
{
  "airplane", "automobile", "bird", "cat", "deer",
  "dog", "frog", "horse", "ship", "truck"
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int stm32_ai_init(void)
{
  int ret;

  syslog(LOG_INFO, "[%s] Initializing AI/TinyML system\n", AI_INIT_TAG);

  /* Initialize AI Agent bridge */

  ret = ai_agent_bridge_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "[%s] Failed to init AI Agent bridge: %d\n",
             AI_INIT_TAG, ret);
      return ret;
    }

  /* Load demo model */

  struct tinyml_model_s model;
  ret = tinyml_load_model(&model, g_demo_model_data,
                          sizeof(g_demo_model_data));
  if (ret < 0)
    {
      syslog(LOG_ERR, "[%s] Failed to load model: %d\n",
             AI_INIT_TAG, ret);
      return ret;
    }

  /* Initialize image classifier */

  ret = img_classifier_init(&model, g_demo_labels, 10);
  if (ret < 0)
    {
      syslog(LOG_ERR, "[%s] Failed to init classifier: %d\n",
             AI_INIT_TAG, ret);
      return ret;
    }

  /* Register tools with AI Agent */

  struct tinyml_tool_config_s tool_config;
  tool_config.model = &model;
  tool_config.img_config = NULL;  /* Will use default */
  tool_config.labels = g_demo_labels;
  tool_config.num_labels = 10;

  ret = ai_agent_register_tinyml_tool(&tool_config);
  if (ret < 0)
    {
      syslog(LOG_ERR, "[%s] Failed to register TinyML tool: %d\n",
             AI_INIT_TAG, ret);
    }

  ret = ai_agent_register_camera_tool();
  if (ret < 0)
    {
      syslog(LOG_WARNING, "[%s] Camera tool not registered\n",
             AI_INIT_TAG);
    }

  ret = ai_agent_register_printer_tool();
  if (ret < 0)
    {
      syslog(LOG_WARNING, "[%s] Printer tool not registered\n",
             AI_INIT_TAG);
    }

  ret = ai_agent_register_lcd_tool();
  if (ret < 0)
    {
      syslog(LOG_WARNING, "[%s] LCD tool not registered\n",
             AI_INIT_TAG);
    }

  syslog(LOG_INFO, "[%s] AI/TinyML system initialized successfully\n",
         AI_INIT_TAG);
  syslog(LOG_INFO, "[%s] TinyML version: %s\n",
         AI_INIT_TAG, tinyml_version());
  syslog(LOG_INFO, "[%s] Arena size: %lu bytes\n",
         AI_INIT_TAG, (unsigned long)tinyml_get_arena_size(&model));

  return OK;
}
