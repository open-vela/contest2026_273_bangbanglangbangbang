/****************************************************************************
 * include/nuttx/ai/cmsis_nn_wrapper.h
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

#ifndef __INCLUDE_NUTTX_AI_CMSIS_NN_WRAPPER_H
#define __INCLUDE_NUTTX_AI_CMSIS_NN_WRAPPER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* CMSIS-NN error codes */

#define CMSIS_NN_SUCCESS          0
#define CMSIS_NN_ARG_ERROR       -1
#define CMSIS_NN_NO_IMPL_ERROR   -2

/* Activation types */

#define CMSIS_NN_SIGMOID         0
#define CMSIS_NN_TANH            1
#define CMSIS_NN_NONE            2

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Convolution parameters */

struct cmsis_nn_conv_params_s
{
  int32_t input_offset;
  int32_t output_offset;
  int32_t stride;
  int32_t padding;
  int32_t dilation;
  int32_t activation_min;
  int32_t activation_max;
};

/* Pooling parameters */

struct cmsis_nn_pool_params_s
{
  int32_t stride;
  int32_t padding;
  int32_t activation_min;
  int32_t activation_max;
};

/* Fully connected parameters */

struct cmsis_nn_fc_params_s
{
  int32_t input_offset;
  int32_t output_offset;
  int32_t filter_offset;
  int32_t activation_min;
  int32_t activation_max;
};

/* Tensor dimensions */

struct cmsis_nn_dims_s
{
  int32_t n;  /* Batch */
  int32_t h;  /* Height */
  int32_t w;  /* Width */
  int32_t c;  /* Channels */
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* Convolution operations */

int32_t cmsis_nn_convolve_s8(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

int32_t cmsis_nn_depthwise_convolve_s8(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

/* Pooling operations */

int32_t cmsis_nn_max_pool_s8(
    FAR const struct cmsis_nn_pool_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

int32_t cmsis_nn_avg_pool_s8(
    FAR const struct cmsis_nn_pool_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

/* Fully connected */

int32_t cmsis_nn_fully_connected_s8(
    FAR const struct cmsis_nn_fc_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

/* Activation functions */

int32_t cmsis_nn_relu_s8(FAR int8_t *data, uint32_t size);
int32_t cmsis_nn_softmax_s8(FAR const int8_t *input, uint32_t size,
                            FAR int8_t *output);

/* Helium (MVE) accelerated versions */

#ifdef CONFIG_AI_CMSIS_NN_HELUM
int32_t cmsis_nn_convolve_s8_mve(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);

int32_t cmsis_nn_fully_connected_s8_mve(
    FAR const struct cmsis_nn_fc_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_AI_CMSIS_NN_WRAPPER_H */
