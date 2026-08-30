/****************************************************************************
 * include/nuttx/ai/ai_agent_bridge.h
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

#ifndef __INCLUDE_NUTTX_AI_AI_AGENT_BRIDGE_H
#define __INCLUDE_NUTTX_AI_AI_AGENT_BRIDGE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/ai/tinyml.h>
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* AI Agent Tool types */

#define AI_AGENT_TOOL_TINYML    0
#define AI_AGENT_TOOL_CAMERA    1
#define AI_AGENT_TOOL_PRINTER   2
#define AI_AGENT_TOOL_LCD       3

/* AI Agent Event types */

#define AI_AGENT_EVENT_RESULT   0
#define AI_AGENT_EVENT_ERROR    1
#define AI_AGENT_EVENT_STATUS   2

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* AI Agent Tool descriptor */

struct ai_agent_tool_s
{
  uint8_t type;
  char name[32];
  char description[128];

  CODE int (*init)(FAR void *config);
  CODE int (*execute)(FAR const char *input, FAR char *output,
                      uint32_t output_size);
  CODE void (*deinit)(void);
};

/* AI Agent Event */

struct ai_agent_event_s
{
  uint8_t type;
  char message[256];
  FAR void *data;
  uint32_t data_size;
};

/* Callback for AI Agent events */

typedef CODE void (*ai_agent_event_cb_t)(
    FAR struct ai_agent_event_s *event, FAR void *arg);

/* TinyML Tool configuration */

struct tinyml_tool_config_s
{
  FAR struct tinyml_model_s *model;
  FAR struct tinyml_img_config_s *img_config;
  FAR const char **labels;
  uint32_t num_labels;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* AI Agent Bridge initialization */

int ai_agent_bridge_init(void);
void ai_agent_bridge_deinit(void);

/* Register TinyML tool with AI Agent */

int ai_agent_register_tinyml_tool(
    FAR struct tinyml_tool_config_s *config);

/* Register camera tool */

int ai_agent_register_camera_tool(void);

/* Register printer tool */

int ai_agent_register_printer_tool(void);

/* Register LCD display tool */

int ai_agent_register_lcd_tool(void);

/* Execute TinyML inference via AI Agent */

int ai_agent_run_tinyml_inference(
    FAR const char *input_image_path,
    FAR struct tinyml_result_s *result);

/* Print result via AI Agent */

int ai_agent_print_result(FAR struct tinyml_result_s *result);

/* Display result on LCD via AI Agent */

int ai_agent_display_result(FAR struct tinyml_result_s *result);

/* Event handling */

int ai_agent_register_event_callback(
    ai_agent_event_cb_t callback, FAR void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_AI_AI_AGENT_BRIDGE_H */
