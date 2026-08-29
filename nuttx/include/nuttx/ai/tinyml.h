/****************************************************************************
 * include/nuttx/ai/tinyml.h
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

#ifndef __INCLUDE_NUTTX_AI_TINYML_H
#define __INCLUDE_NUTTX_AI_TINYML_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* TinyML version */

#define TINYML_VERSION_MAJOR  1
#define TINYML_VERSION_MINOR  0
#define TINYML_VERSION_PATCH  0

/* Maximum dimensions */

#define TINYML_MAX_DIMS       8
#define TINYML_MAX_LAYERS     32
#define TINYML_MAX_NAME_LEN   32

/* Tensor types */

#define TINYML_TYPE_INT8      0
#define TINYML_TYPE_INT16     1
#define TINYML_TYPE_INT32     2
#define TINYML_TYPE_FLOAT32   3
#define TINYML_TYPE_BOOL      4

/* Operation types */

#define TINYML_OP_CONV2D      0
#define TINYML_OP_DEPTHWISE   1
#define TINYML_OP_FULLY_CONN  2
#define TINYML_OP_POOL_MAX    3
#define TINYML_OP_POOL_AVG    4
#define TINYML_OP_RELU        5
#define TINYML_OP_SOFTMAX     6
#define TINYML_OP_ADD         7
#define TINYML_OP_MUL         8
#define TINYML_OP_RESHAPE     9
#define TINYML_OP_CONCAT      10

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Tensor descriptor */

struct tinyml_tensor_s
{
  char name[TINYML_MAX_NAME_LEN];
  uint8_t type;
  uint8_t ndims;
  uint32_t dims[TINYML_MAX_DIMS];
  uint32_t size;          /* Total size in bytes */
  FAR void *data;
};

/* Layer descriptor */

struct tinyml_layer_s
{
  uint8_t type;
  char name[TINYML_MAX_NAME_LEN];
  uint8_t num_inputs;
  uint8_t num_outputs;
  FAR struct tinyml_tensor_s *inputs[TINYML_MAX_DIMS];
  FAR struct tinyml_tensor_s *outputs[TINYML_MAX_DIMS];
  FAR void *params;       /* Layer-specific parameters */
};

/* Model descriptor */

struct tinyml_model_s
{
  char name[TINYML_MAX_NAME_LEN];
  uint8_t num_layers;
  uint8_t num_tensors;
  FAR struct tinyml_layer_s *layers;
  FAR struct tinyml_tensor_s *tensors;
  FAR void *arena;        /* Memory arena */
  uint32_t arena_size;
};

/* Inference result */

struct tinyml_result_s
{
  int class_id;
  float confidence;
  FAR float *scores;
  uint32_t num_classes;
};

/* Callback for async inference */

typedef CODE void (*tinyml_callback_t)(
    FAR struct tinyml_result_s *result, FAR void *arg);

/* TinyML engine operations */

struct tinyml_ops_s
{
  CODE int (*init)(FAR struct tinyml_model_s *model);
  CODE int (*invoke)(FAR struct tinyml_model_s *model);
  CODE int (*reset)(FAR struct tinyml_model_s *model);
  CODE void (*deinit)(FAR struct tinyml_model_s *model);
};

/* Image classification specific */

struct tinyml_img_config_s
{
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint8_t input_type;     /* TINYML_TYPE_INT8 or FLOAT32 */
  float scale;
  int32_t zero_point;
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

/* Model management */

int tinyml_load_model(FAR struct tinyml_model_s *model,
                      FAR const uint8_t *data, uint32_t size);
int tinyml_free_model(FAR struct tinyml_model_s *model);

/* Inference */

int tinyml_invoke(FAR struct tinyml_model_s *model);
int tinyml_invoke_async(FAR struct tinyml_model_s *model,
                        tinyml_callback_t callback, FAR void *arg);

/* Tensor operations */

FAR struct tinyml_tensor_s *tinyml_get_input(
    FAR struct tinyml_model_s *model, uint8_t index);
FAR struct tinyml_tensor_s *tinyml_get_output(
    FAR struct tinyml_model_s *model, uint8_t index);
int tinyml_set_input(FAR struct tinyml_tensor_s *tensor,
                     FAR const void *data, uint32_t size);
int tinyml_get_output_data(FAR struct tinyml_tensor_s *tensor,
                           FAR void *data, uint32_t size);

/* Image classification helpers */

int tinyml_img_classify(FAR struct tinyml_model_s *model,
                        FAR const uint8_t *image,
                        FAR struct tinyml_img_config_s *config,
                        FAR struct tinyml_result_s *result);

/* Utility */

const char *tinyml_version(void);
uint32_t tinyml_get_arena_size(FAR struct tinyml_model_s *model);

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_AI_TINYML_H */
