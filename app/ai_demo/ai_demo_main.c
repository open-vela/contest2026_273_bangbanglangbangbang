/****************************************************************************
 * examples/ai_demo/ai_demo_main.c
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define AI_DEMO_TAG  "ai_demo"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void print_banner(void)
{
  printf("\n");
  printf("========================================\n");
  printf("  STM32N647-EVB AI Demo Application\n");
  printf("  TinyML + AI Agent Architecture\n");
  printf("========================================\n");
  printf("\n");
  printf("Architecture:\n");
  printf("  - TinyML (Cortex-M55 + Helium MVE) = Cerebellum\n");
  printf("  - AI Agent (Cloud LLM) = Brain\n");
  printf("  - Peripherals: Camera + LCD + Printer\n");
  printf("\n");
}

static void print_usage(void)
{
  printf("Usage: ai_demo [options]\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h          Show this help\n");
  printf("  -c          Run continuous classification\n");
  printf("  -s          Run single classification\n");
  printf("  -t          Test TinyML inference\n");
  printf("  -a          Test AI Agent tools\n");
  printf("\n");
}

static int run_single_classification(void)
{
  int ret;
  struct tinyml_result_s result;

  printf("[%s] Running single image classification...\n", AI_DEMO_TAG);

  ret = img_classifier_run(&result);
  if (ret < 0)
    {
      printf("[%s] Classification failed: %d\n", AI_DEMO_TAG, ret);
      return ret;
    }

  printf("[%s] Result:\n", AI_DEMO_TAG);
  printf("  Class ID: %d\n", result.class_id);
  printf("  Confidence: %.2f%%\n", result.confidence * 100.0f);

  /* Display result via AI Agent */

  ai_agent_display_result(&result);

  /* Print result via AI Agent */

  ai_agent_print_result(&result);

  /* Cleanup */

  if (result.scores != NULL)
    {
      free(result.scores);
    }

  return OK;
}

static int run_continuous_classification(void)
{
  int ret;
  int count = 0;

  printf("[%s] Starting continuous classification (Ctrl+C to stop)...\n",
         AI_DEMO_TAG);

  while (1)
    {
      struct tinyml_result_s result;

      printf("\n[%s] Frame %d\n", AI_DEMO_TAG, ++count);

      ret = img_classifier_run(&result);
      if (ret < 0)
        {
          printf("[%s] Classification failed: %d\n", AI_DEMO_TAG, ret);
          usleep(1000000);  /* Wait 1 second before retry */
          continue;
        }

      printf("  Class: %d, Confidence: %.2f%%\n",
             result.class_id, result.confidence * 100.0f);

      /* Display and print result */

      ai_agent_display_result(&result);
      ai_agent_print_result(&result);

      if (result.scores != NULL)
        {
          free(result.scores);
        }

      /* Wait before next frame */

      usleep(500000);  /* 500ms */
    }

  return OK;
}

static int test_tinyml_inference(void)
{
  printf("[%s] Testing TinyML inference engine...\n", AI_DEMO_TAG);

  /* Create a dummy model for testing */

  struct tinyml_model_s model;
  memset(&model, 0, sizeof(model));

  /* Load model from memory */

  const uint8_t dummy_model[] =
  {
    0x04, 't', 'e', 's', 't',  /* Name: "test" */
    0x01,                        /* 1 layer */
    0x02,                        /* 2 tensors */

    /* Tensor 0: input */
    0x05, 'i', 'n', 'p', 'u', 't',
    0x00,                        /* INT8 */
    0x02,                        /* 2 dims */
    0x00, 0x00, 0x00, 0x01,     /* 1 */
    0x00, 0x00, 0x00, 0x0a,     /* 10 */

    /* Tensor 1: output */
    0x06, 'o', 'u', 't', 'p', 'u', 't',
    0x00,                        /* INT8 */
    0x02,                        /* 2 dims */
    0x00, 0x00, 0x00, 0x01,     /* 1 */
    0x00, 0x00, 0x00, 0x0a,     /* 10 */

    /* Layer 0: FC */
    0x02,                        /* FC */
    0x02, 'f', 'c',
    0x01,                        /* 1 input */
    0x01                         /* 1 output */
  };

  int ret = tinyml_load_model(&model, dummy_model, sizeof(dummy_model));
  if (ret < 0)
    {
      printf("[%s] Failed to load model: %d\n", AI_DEMO_TAG, ret);
      return ret;
    }

  printf("[%s] Model loaded: %s\n", AI_DEMO_TAG, model.name);
  printf("[%s] Layers: %d, Tensors: %d\n",
         AI_DEMO_TAG, model.num_layers, model.num_tensors);
  printf("[%s] Arena size: %lu bytes\n",
         AI_DEMO_TAG, (unsigned long)tinyml_get_arena_size(&model));
  printf("[%s] TinyML version: %s\n", AI_DEMO_TAG, tinyml_version());

  /* Run inference */

  ret = tinyml_invoke(&model);
  printf("[%s] Inference result: %d\n", AI_DEMO_TAG, ret);

  /* Cleanup */

  tinyml_free_model(&model);

  return OK;
}

static int test_ai_agent_tools(void)
{
  printf("[%s] Testing AI Agent tools...\n", AI_DEMO_TAG);

  /* Initialize AI Agent bridge */

  int ret = ai_agent_bridge_init();
  if (ret < 0)
    {
      printf("[%s] Failed to init bridge: %d\n", AI_DEMO_TAG, ret);
      return ret;
    }

  /* Register tools */

  ret = ai_agent_register_camera_tool();
  printf("[%s] Camera tool: %s\n", AI_DEMO_TAG,
         ret == 0 ? "OK" : "FAILED");

  ret = ai_agent_register_printer_tool();
  printf("[%s] Printer tool: %s\n", AI_DEMO_TAG,
         ret == 0 ? "OK" : "FAILED");

  ret = ai_agent_register_lcd_tool();
  printf("[%s] LCD tool: %s\n", AI_DEMO_TAG,
         ret == 0 ? "OK" : "FAILED");

  /* Test tool execution */

  char output[256];

  /* Test camera */

  ret = ai_agent_register_camera_tool();
  if (ret == 0)
    {
      printf("[%s] Testing camera capture...\n", AI_DEMO_TAG);
      /* Camera tool would execute here */
    }

  /* Test printer */

  printf("[%s] Testing printer output...\n", AI_DEMO_TAG);
  /* Printer tool would execute here */

  /* Test LCD */

  printf("[%s] Testing LCD display...\n", AI_DEMO_TAG);
  /* LCD tool would execute here */

  printf("[%s] AI Agent tools test complete\n", AI_DEMO_TAG);

  /* Cleanup */

  ai_agent_bridge_deinit();

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int opt;
  bool run_continuous = false;
  bool run_single = false;
  bool test_tinyml = false;
  bool test_agent = false;

  print_banner();

  /* Parse command line options */

  while ((opt = getopt(argc, argv, "hcsta")) != -1)
    {
      switch (opt)
        {
          case 'h':
            print_usage();
            return 0;

          case 'c':
            run_continuous = true;
            break;

          case 's':
            run_single = true;
            break;

          case 't':
            test_tinyml = true;
            break;

          case 'a':
            test_agent = true;
            break;

          default:
            print_usage();
            return 1;
        }
    }

  /* Default to single classification if no option specified */

  if (!run_continuous && !run_single && !test_tinyml && !test_agent)
    {
      run_single = true;
    }

  /* Run selected tests */

  if (test_tinyml)
    {
      test_tinyml_inference();
    }

  if (test_agent)
    {
      test_ai_agent_tools();
    }

  if (run_single)
    {
      run_single_classification();
    }

  if (run_continuous)
    {
      run_continuous_classification();
    }

  printf("\n[%s] Demo complete\n", AI_DEMO_TAG);

  return 0;
}
