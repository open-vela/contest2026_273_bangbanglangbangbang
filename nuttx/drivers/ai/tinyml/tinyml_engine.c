/****************************************************************************
 * drivers/ai/tinyml/tinyml_engine.c
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
#include <nuttx/ai/cmsis_nn_wrapper.h>
#include <nuttx/kmalloc.h>
#include <nuttx/log.h>
#include <string.h>
#include <stdlib.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TINYML_LOG_TAG "tinyml"
#define TINYML_ARENA_ALIGNMENT 16

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int tinyml_parse_tensor(FAR const uint8_t **ptr,
                               FAR struct tinyml_tensor_s *tensor)
{
  FAR const uint8_t *p = *ptr;

  /* Read name length and name */

  uint8_t name_len = *p++;
  if (name_len >= TINYML_MAX_NAME_LEN)
    {
      return -EINVAL;
    }

  memcpy(tensor->name, p, name_len);
  tensor->name[name_len] = '\0';
  p += name_len;

  /* Read type and dimensions */

  tensor->type = *p++;
  tensor->ndims = *p++;

  if (tensor->ndims > TINYML_MAX_DIMS)
    {
      return -EINVAL;
    }

  /* Read dimensions */

  for (int i = 0; i < tensor->ndims; i++)
    {
      tensor->dims[i] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
      p += 4;
    }

  /* Calculate total size */

  uint32_t elem_size = 1;
  switch (tensor->type)
    {
      case TINYML_TYPE_INT8:
        elem_size = 1;
        break;
      case TINYML_TYPE_INT16:
        elem_size = 2;
        break;
      case TINYML_TYPE_INT32:
      case TINYML_TYPE_FLOAT32:
        elem_size = 4;
        break;
      case TINYML_TYPE_BOOL:
        elem_size = 1;
        break;
      default:
        return -EINVAL;
    }

  tensor->size = 1;
  for (int i = 0; i < tensor->ndims; i++)
    {
      tensor->size *= tensor->dims[i];
    }

  tensor->size *= elem_size;
  tensor->data = NULL;

  *ptr = p;
  return OK;
}

static int tinyml_parse_layer(FAR const uint8_t **ptr,
                              FAR struct tinyml_layer_s *layer)
{
  FAR const uint8_t *p = *ptr;

  /* Read layer type */

  layer->type = *p++;

  /* Read name */

  uint8_t name_len = *p++;
  if (name_len >= TINYML_MAX_NAME_LEN)
    {
      return -EINVAL;
    }

  memcpy(layer->name, p, name_len);
  layer->name[name_len] = '\0';
  p += name_len;

  /* Read input/output counts */

  layer->num_inputs = *p++;
  layer->num_outputs = *p++;

  /* Clear pointers */

  for (int i = 0; i < TINYML_MAX_DIMS; i++)
    {
      layer->inputs[i] = NULL;
      layer->outputs[i] = NULL;
    }

  layer->params = NULL;
  *ptr = p;
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int tinyml_load_model(FAR struct tinyml_model_s *model,
                      FAR const uint8_t *data, uint32_t size)
{
  if (model == NULL || data == NULL || size == 0)
    {
      return -EINVAL;
    }

  FAR const uint8_t *ptr = data;

  /* Read model header */

  uint8_t name_len = *ptr++;
  if (name_len >= TINYML_MAX_NAME_LEN)
    {
      return -EINVAL;
    }

  memcpy(model->name, ptr, name_len);
  model->name[name_len] = '\0';
  ptr += name_len;

  model->num_layers = *ptr++;
  model->num_tensors = *ptr++;

  /* Allocate layers and tensors */

  model->layers = kmm_zalloc(model->num_layers *
                              sizeof(struct tinyml_layer_s));
  if (model->layers == NULL)
    {
      return -ENOMEM;
    }

  model->tensors = kmm_zalloc(model->num_tensors *
                               sizeof(struct tinyml_tensor_s));
  if (model->tensors == NULL)
    {
      kmm_free(model->layers);
      return -ENOMEM;
    }

  /* Parse tensors */

  for (int i = 0; i < model->num_tensors; i++)
    {
      int ret = tinyml_parse_tensor(&ptr, &model->tensors[i]);
      if (ret < 0)
        {
          goto err_free;
        }
    }

  /* Parse layers */

  for (int i = 0; i < model->num_layers; i++)
    {
      int ret = tinyml_parse_layer(&ptr, &model->layers[i]);
      if (ret < 0)
        {
          goto err_free;
        }
    }

  /* Allocate memory arena */

  model->arena_size = 0;
  for (int i = 0; i < model->num_tensors; i++)
    {
      model->arena_size += model->tensors[i].size +
                           TINYML_ARENA_ALIGNMENT;
    }

  model->arena = kmm_memalign(TINYML_ARENA_ALIGNMENT,
                               model->arena_size);
  if (model->arena == NULL)
    {
      goto err_free;
    }

  /* Assign tensor data pointers */

  FAR uint8_t *arena_ptr = (FAR uint8_t *)model->arena;
  for (int i = 0; i < model->num_tensors; i++)
    {
      model->tensors[i].data = arena_ptr;
      arena_ptr += ALIGN_UP(model->tensors[i].size,
                            TINYML_ARENA_ALIGNMENT);
    }

  ai_info("Model '%s' loaded: %d layers, %d tensors, arena %lu bytes\n",
          model->name, model->num_layers, model->num_tensors,
          (unsigned long)model->arena_size);

  return OK;

err_free:
  kmm_free(model->tensors);
  kmm_free(model->layers);
  return -ENOMEM;
}

int tinyml_free_model(FAR struct tinyml_model_s *model)
{
  if (model == NULL)
    {
      return -EINVAL;
    }

  if (model->arena != NULL)
    {
      kmm_free(model->arena);
    }

  if (model->layers != NULL)
    {
      kmm_free(model->layers);
    }

  if (model->tensors != NULL)
    {
      kmm_free(model->tensors);
    }

  memset(model, 0, sizeof(struct tinyml_model_s));
  return OK;
}

int tinyml_invoke(FAR struct tinyml_model_s *model)
{
  if (model == NULL)
    {
      return -EINVAL;
    }

  /* Execute each layer in order */

  for (int i = 0; i < model->num_layers; i++)
    {
      FAR struct tinyml_layer_s *layer = &model->layers[i];
      int ret = OK;

      switch (layer->type)
        {
          case TINYML_OP_CONV2D:
            /* Will be implemented with CMSIS-NN */

            break;

          case TINYML_OP_DEPTHWISE:
            break;

          case TINYML_OP_FULLY_CONN:
            break;

          case TINYML_OP_POOL_MAX:
            break;

          case TINYML_OP_POOL_AVG:
            break;

          case TINYML_OP_RELU:
            break;

          case TINYML_OP_SOFTMAX:
            break;

          default:
            ai_err("Unknown layer type: %d\n", layer->type);
            return -ENOTSUP;
        }

      if (ret < 0)
        {
          ai_err("Layer '%s' failed: %d\n", layer->name, ret);
          return ret;
        }
    }

  return OK;
}

FAR struct tinyml_tensor_s *tinyml_get_input(
    FAR struct tinyml_model_s *model, uint8_t index)
{
  if (model == NULL || index >= model->num_layers)
    {
      return NULL;
    }

  /* Return first input of first layer (model input) */

  return model->layers[0].inputs[index];
}

FAR struct tinyml_tensor_s *tinyml_get_output(
    FAR struct tinyml_model_s *model, uint8_t index)
{
  if (model == NULL || index >= model->num_layers)
    {
      return NULL;
    }

  /* Return first output of last layer (model output) */

  return model->layers[model->num_layers - 1].outputs[index];
}

int tinyml_set_input(FAR struct tinyml_tensor_s *tensor,
                     FAR const void *data, uint32_t size)
{
  if (tensor == NULL || data == NULL || size > tensor->size)
    {
      return -EINVAL;
    }

  memcpy(tensor->data, data, size);
  return OK;
}

int tinyml_get_output_data(FAR struct tinyml_tensor_s *tensor,
                           FAR void *data, uint32_t size)
{
  if (tensor == NULL || data == NULL || size < tensor->size)
    {
      return -EINVAL;
    }

  memcpy(data, tensor->data, tensor->size);
  return OK;
}

int tinyml_img_classify(FAR struct tinyml_model_s *model,
                        FAR const uint8_t *image,
                        FAR struct tinyml_img_config_s *config,
                        FAR struct tinyml_result_s *result)
{
  if (model == NULL || image == NULL || config == NULL || result == NULL)
    {
      return -EINVAL;
    }

  FAR struct tinyml_tensor_s *input = tinyml_get_input(model, 0);
  if (input == NULL)
    {
      return -EINVAL;
    }

  /* Preprocess image - normalize to int8 if needed */

  if (config->input_type == TINYML_TYPE_INT8)
    {
      FAR int8_t *input_data = (FAR int8_t *)input->data;
      uint32_t pixels = config->width * config->height * config->channels;

      for (uint32_t i = 0; i < pixels; i++)
        {
          float normalized = ((float)image[i] / 255.0f - 0.5f) /
                             config->scale;
          int32_t quantized = (int32_t)(normalized) + config->zero_point;

          if (quantized > 127)
            {
              quantized = 127;
            }
          else if (quantized < -128)
            {
              quantized = -128;
            }

          input_data[i] = (int8_t)quantized;
        }
    }
  else
    {
      /* Direct copy for float input */

      memcpy(input->data, image, config->width * config->height *
             config->channels * sizeof(float));
    }

  /* Run inference */

  int ret = tinyml_invoke(model);
  if (ret < 0)
    {
      return ret;
    }

  /* Get output and find max class */

  FAR struct tinyml_tensor_s *output = tinyml_get_output(model, 0);
  if (output == NULL)
    {
      return -EINVAL;
    }

  result->num_classes = output->dims[output->ndims - 1];
  result->scores = (FAR float *)kmm_zalloc(result->num_classes *
                                           sizeof(float));
  if (result->scores == NULL)
    {
      return -ENOMEM;
    }

  /* Convert output to scores */

  if (output->type == TINYML_TYPE_INT8)
    {
      FAR int8_t *out_data = (FAR int8_t *)output->data;
      float max_score = -1e9f;

      for (uint32_t i = 0; i < result->num_classes; i++)
        {
          result->scores[i] = ((float)out_data[i] - config->zero_point) *
                              config->scale;
          if (result->scores[i] > max_score)
            {
              max_score = result->scores[i];
              result->class_id = i;
            }
        }

      result->confidence = max_score;
    }
  else
    {
      FAR float *out_data = (FAR float *)output->data;
      float max_score = -1e9f;

      for (uint32_t i = 0; i < result->num_classes; i++)
        {
          result->scores[i] = out_data[i];
          if (out_data[i] > max_score)
            {
              max_score = out_data[i];
              result->class_id = i;
            }
        }

      result->confidence = max_score;
    }

  return OK;
}

const char *tinyml_version(void)
{
  static char version[16];
  snprintf(version, sizeof(version), "%d.%d.%d",
           TINYML_VERSION_MAJOR, TINYML_VERSION_MINOR,
           TINYML_VERSION_PATCH);
  return version;
}

uint32_t tinyml_get_arena_size(FAR struct tinyml_model_s *model)
{
  if (model == NULL)
    {
      return 0;
    }

  return model->arena_size;
}
