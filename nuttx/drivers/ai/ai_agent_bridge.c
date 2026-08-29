/****************************************************************************
 * drivers/ai/ai_agent_bridge.c
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
#include <nuttx/ai/ai_agent_bridge.h>
#include <nuttx/ai/tinyml.h>
#include <nuttx/kmalloc.h>
#include <nuttx/log.h>
#include <string.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define AGENT_BRIDGE_TAG  "ai_bridge"
#define MAX_TOOLS         8

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct ai_agent_tool_s g_tools[MAX_TOOLS];
static uint8_t g_num_tools = 0;
static ai_agent_event_cb_t g_event_callback = NULL;
static FAR void *g_event_arg = NULL;
static bool g_initialized = false;

/* TinyML tool instance */

static struct tinyml_model_s *g_tinyml_model = NULL;
static struct tinyml_img_config_s *g_img_config = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void notify_event(uint8_t type, FAR const char *message,
                         FAR void *data, uint32_t data_size)
{
  if (g_event_callback == NULL)
    {
      return;
    }

  struct ai_agent_event_s event;
  event.type = type;
  strncpy(event.message, message, sizeof(event.message) - 1);
  event.message[sizeof(event.message) - 1] = '\0';
  event.data = data;
  event.data_size = data_size;

  g_event_callback(&event, g_event_arg);
}

static int tinyml_tool_init(FAR void *config)
{
  FAR struct tinyml_tool_config_s *cfg =
      (FAR struct tinyml_tool_config_s *)config;

  if (cfg == NULL || cfg->model == NULL)
    {
      return -EINVAL;
    }

  g_tinyml_model = cfg->model;
  g_img_config = cfg->img_config;

  ai_info("TinyML tool initialized\n");
  return OK;
}

static int tinyml_tool_execute(FAR const char *input,
                               FAR char *output,
                               uint32_t output_size)
{
  if (g_tinyml_model == NULL || input == NULL || output == NULL)
    {
      return -EINVAL;
    }

  /* Run image classification */

  struct tinyml_result_s result;
  memset(&result, 0, sizeof(result));

  /* In real implementation, load image from input path */

  int ret = tinyml_img_classify(g_tinyml_model, (FAR const uint8_t *)input,
                                g_img_config, &result);
  if (ret < 0)
    {
      snprintf(output, output_size, "Error: classification failed (%d)", ret);
      notify_event(AI_AGENT_EVENT_ERROR, output, NULL, 0);
      return ret;
    }

  /* Format result */

  if (g_img_config->labels != NULL &&
      result.class_id < g_img_config->num_labels)
    {
      snprintf(output, output_size,
               "{\"class\":\"%s\",\"confidence\":%.2f,\"class_id\":%d}",
               g_img_config->labels[result.class_id],
               result.confidence,
               result.class_id);
    }
  else
    {
      snprintf(output, output_size,
               "{\"class_id\":%d,\"confidence\":%.2f}",
               result.class_id,
               result.confidence);
    }

  notify_event(AI_AGENT_EVENT_RESULT, output, &result, sizeof(result));

  /* Free result scores */

  if (result.scores != NULL)
    {
      kmm_free(result.scores);
    }

  return OK;
}

static void tinyml_tool_deinit(void)
{
  g_tinyml_model = NULL;
  g_img_config = NULL;
  ai_info("TinyML tool deinitialized\n");
}

static int camera_tool_init(FAR void *config)
{
  /* Camera initialization handled by img_classifier */

  ai_info("Camera tool initialized\n");
  return OK;
}

static int camera_tool_execute(FAR const char *input,
                               FAR char *output,
                               uint32_t output_size)
{
  /* Capture frame and save to file or return raw data */

  snprintf(output, output_size, "Camera capture: OK");
  notify_event(AI_AGENT_EVENT_STATUS, "Frame captured", NULL, 0);
  return OK;
}

static void camera_tool_deinit(void)
{
  ai_info("Camera tool deinitialized\n");
}

static int printer_tool_init(FAR void *config)
{
  ai_info("Printer tool initialized\n");
  return OK;
}

static int printer_tool_execute(FAR const char *input,
                                FAR char *output,
                                uint32_t output_size)
{
  if (input == NULL)
    {
      return -EINVAL;
    }

  /* Print the input text */

  ai_info("Printing: %s\n", input);
  snprintf(output, output_size, "Printed: %s", input);
  notify_event(AI_AGENT_EVENT_STATUS, "Print complete", NULL, 0);
  return OK;
}

static void printer_tool_deinit(void)
{
  ai_info("Printer tool deinitialized\n");
}

static int lcd_tool_init(FAR void *config)
{
  ai_info("LCD tool initialized\n");
  return OK;
}

static int lcd_tool_execute(FAR const char *input,
                            FAR char *output,
                            uint32_t output_size)
{
  if (input == NULL)
    {
      return -EINVAL;
    }

  /* Display text on LCD */

  ai_info("LCD display: %s\n", input);
  snprintf(output, output_size, "Displayed: %s", input);
  notify_event(AI_AGENT_EVENT_STATUS, "Display updated", NULL, 0);
  return OK;
}

static void lcd_tool_deinit(void)
{
  ai_info("LCD tool deinitialized\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int ai_agent_bridge_init(void)
{
  if (g_initialized)
    {
      return OK;
    }

  memset(g_tools, 0, sizeof(g_tools));
  g_num_tools = 0;

  g_initialized = true;
  ai_info("AI Agent bridge initialized\n");

  return OK;
}

void ai_agent_bridge_deinit(void)
{
  /* Deinitialize all tools */

  for (int i = 0; i < g_num_tools; i++)
    {
      if (g_tools[i].deinit != NULL)
        {
          g_tools[i].deinit();
        }
    }

  g_num_tools = 0;
  g_initialized = false;
  ai_info("AI Agent bridge deinitialized\n");
}

int ai_agent_register_tinyml_tool(
    FAR struct tinyml_tool_config_s *config)
{
  if (!g_initialized || config == NULL)
    {
      return -EINVAL;
    }

  if (g_num_tools >= MAX_TOOLS)
    {
      return -ENOMEM;
    }

  FAR struct ai_agent_tool_s *tool = &g_tools[g_num_tools];
  tool->type = AI_AGENT_TOOL_TINYML;
  strncpy(tool->name, "tinyml_classifier", sizeof(tool->name) - 1);
  strncpy(tool->description, "TinyML image classification tool",
          sizeof(tool->description) - 1);
  tool->init = tinyml_tool_init;
  tool->execute = tinyml_tool_execute;
  tool->deinit = tinyml_tool_deinit;

  int ret = tool->init(config);
  if (ret < 0)
    {
      return ret;
    }

  g_num_tools++;
  ai_info("Registered TinyML tool\n");

  return OK;
}

int ai_agent_register_camera_tool(void)
{
  if (!g_initialized)
    {
      return -EINVAL;
    }

  if (g_num_tools >= MAX_TOOLS)
    {
      return -ENOMEM;
    }

  FAR struct ai_agent_tool_s *tool = &g_tools[g_num_tools];
  tool->type = AI_AGENT_TOOL_CAMERA;
  strncpy(tool->name, "camera", sizeof(tool->name) - 1);
  strncpy(tool->description, "Camera capture tool",
          sizeof(tool->description) - 1);
  tool->init = camera_tool_init;
  tool->execute = camera_tool_execute;
  tool->deinit = camera_tool_deinit;

  tool->init(NULL);
  g_num_tools++;

  return OK;
}

int ai_agent_register_printer_tool(void)
{
  if (!g_initialized)
    {
      return -EINVAL;
    }

  if (g_num_tools >= MAX_TOOLS)
    {
      return -ENOMEM;
    }

  FAR struct ai_agent_tool_s *tool = &g_tools[g_num_tools];
  tool->type = AI_AGENT_TOOL_PRINTER;
  strncpy(tool->name, "printer", sizeof(tool->name) - 1);
  strncpy(tool->description, "Thermal printer tool",
          sizeof(tool->description) - 1);
  tool->init = printer_tool_init;
  tool->execute = printer_tool_execute;
  tool->deinit = printer_tool_deinit;

  tool->init(NULL);
  g_num_tools++;

  return OK;
}

int ai_agent_register_lcd_tool(void)
{
  if (!g_initialized)
    {
      return -EINVAL;
    }

  if (g_num_tools >= MAX_TOOLS)
    {
      return -ENOMEM;
    }

  FAR struct ai_agent_tool_s *tool = &g_tools[g_num_tools];
  tool->type = AI_AGENT_TOOL_LCD;
  strncpy(tool->name, "lcd_display", sizeof(tool->name) - 1);
  strncpy(tool->description, "LCD display tool",
          sizeof(tool->description) - 1);
  tool->init = lcd_tool_init;
  tool->execute = lcd_tool_execute;
  tool->deinit = lcd_tool_deinit;

  tool->init(NULL);
  g_num_tools++;

  return OK;
}

int ai_agent_run_tinyml_inference(
    FAR const char *input_image_path,
    FAR struct tinyml_result_s *result)
{
  if (!g_initialized || input_image_path == NULL || result == NULL)
    {
      return -EINVAL;
    }

  /* Find TinyML tool */

  for (int i = 0; i < g_num_tools; i++)
    {
      if (g_tools[i].type == AI_AGENT_TOOL_TINYML)
        {
          char output[256];
          int ret = g_tools[i].execute(input_image_path, output,
                                       sizeof(output));
          if (ret < 0)
            {
              return ret;
            }

          /* Parse JSON result */

          /* TODO: Implement JSON parsing */

          return OK;
        }
    }

  return -ENODEV;
}

int ai_agent_print_result(FAR struct tinyml_result_s *result)
{
  if (!g_initialized || result == NULL)
    {
      return -EINVAL;
    }

  /* Find printer tool */

  for (int i = 0; i < g_num_tools; i++)
    {
      if (g_tools[i].type == AI_AGENT_TOOL_PRINTER)
        {
          char input[128];
          char output[128];

          if (g_img_config != NULL && g_img_config->labels != NULL &&
              result->class_id < g_img_config->num_labels)
            {
              snprintf(input, sizeof(input), "Result: %s (%.1f%%)",
                       g_img_config->labels[result->class_id],
                       result->confidence * 100.0f);
            }
          else
            {
              snprintf(input, sizeof(input), "Class: %d (%.1f%%)",
                       result->class_id,
                       result->confidence * 100.0f);
            }

          return g_tools[i].execute(input, output, sizeof(output));
        }
    }

  return -ENODEV;
}

int ai_agent_display_result(FAR struct tinyml_result_s *result)
{
  if (!g_initialized || result == NULL)
    {
      return -EINVAL;
    }

  /* Find LCD tool */

  for (int i = 0; i < g_num_tools; i++)
    {
      if (g_tools[i].type == AI_AGENT_TOOL_LCD)
        {
          char input[128];
          char output[128];

          if (g_img_config != NULL && g_img_config->labels != NULL &&
              result->class_id < g_img_config->num_labels)
            {
              snprintf(input, sizeof(input), "%s: %.1f%%",
                       g_img_config->labels[result->class_id],
                       result->confidence * 100.0f);
            }
          else
            {
              snprintf(input, sizeof(input), "Class %d: %.1f%%",
                       result->class_id,
                       result->confidence * 100.0f);
            }

          return g_tools[i].execute(input, output, sizeof(output));
        }
    }

  return -ENODEV;
}

int ai_agent_register_event_callback(
    ai_agent_event_cb_t callback, FAR void *arg)
{
  g_event_callback = callback;
  g_event_arg = arg;
  return OK;
}
